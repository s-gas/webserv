import requests
import subprocess
import pytest
import os
import re

# --- Configuration based on test25.conf ---
HOST = "127.0.0.1"
TIMEOUT = 3 # Fail fast if the server hangs

@pytest.fixture(scope="session", autouse=True)
def check_server_is_up():
    """Ensures the server is running before tests start."""
    try:
        requests.get(f"http://{HOST}:1024", timeout=TIMEOUT)
    except requests.exceptions.ConnectionError:
        pytest.exit("❌ Webserv is not running! Please start the server before running E2E tests.")

# -----------------------------------------------------------------------------
# 1. Test Virtual Server Isolation
# -----------------------------------------------------------------------------
def test_port_1024():
    response = requests.get(f"http://{HOST}:1024/", timeout=TIMEOUT)
    # Could be 200 (OK) or 403 (Forbidden if no index file), both mean the port is listening
    assert response.status_code in [200, 403], f"Port 1024 returned unexpected status: {response.status_code}"

def test_port_1025_white_route():
    response = requests.get(f"http://{HOST}:1025/white/", timeout=TIMEOUT)
    assert response.status_code in [200, 403], f"Port 1025 returned unexpected status: {response.status_code}"

def test_port_1026_green_route():
    response = requests.get(f"http://{HOST}:1026/", timeout=TIMEOUT)
    assert response.status_code in [200, 403], f"Port 1026 returned unexpected status: {response.status_code}"

# -----------------------------------------------------------------------------
# 2. Test Client Max Body Size
# -----------------------------------------------------------------------------
def test_body_size_small_payload():
    url = f"http://{HOST}:1024/submit/"
    payload = "hello" # 5 bytes (Limit is 20)
    response = requests.post(url, data=payload, timeout=TIMEOUT)
    assert response.status_code in [200, 201, 204], f"Expected success for small payload, got {response.status_code}"

def test_body_size_large_payload():
    url = f"http://{HOST}:1024/submit/"
    payload = "this_is_a_very_long_sentence_over_20_bytes" 
    response = requests.post(url, data=payload, timeout=TIMEOUT)
    assert response.status_code == 413, f"Expected 413 Payload Too Large, got {response.status_code}"

# -----------------------------------------------------------------------------
# 3. Test CGI Interpreters
# -----------------------------------------------------------------------------
@pytest.mark.parametrize("script", ["test.py", "test.sh"])
def test_supported_cgi_scripts(script):
    url = f"http://{HOST}:1024/cgi-bin/{script}"
    response = requests.get(url, timeout=TIMEOUT)
    assert response.status_code == 200, f"Failed to execute supported CGI script {script}"

def test_unsupported_cgi_script():
    url = f"http://{HOST}:1024/cgi-bin/test.php"
    response = requests.get(url, timeout=TIMEOUT)
    assert response.status_code != 200, "Unsupported CGI script (.php) should not return 200 OK"

def test_siege_stress_load():
    """Runs Siege as a subprocess and asserts that 0 transactions failed."""
    urls_file = ".siege_urls.txt"
    with open(urls_file, "w") as f:
        f.write(f"http://{HOST}:1024/\n")
        f.write(f"http://{HOST}:1024/cgi-bin/test.py\n")
        f.write(f"http://{HOST}:1025/white/\n")
        f.write(f"http://{HOST}:1026/\n")

    # Run siege for 30 seconds with 20 concurrent users
    result = subprocess.run(
        ["siege", "-c20", "-t30S", f"-f{urls_file}"],
        capture_output=True,
        text=True
    )
    
    import os
    os.remove(urls_file)
    
    output = result.stderr + result.stdout
    
    # Strip ALL whitespace (spaces, tabs, newlines) from the output to make the assertion bulletproof
    clean_output = re.sub(r'\s+', '', output)
    
    assert '"failed_transactions":0' in clean_output, \
        f"Siege test failed or dropped connections!\nOutput:\n{output}"
