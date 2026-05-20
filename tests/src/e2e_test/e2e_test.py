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
# 1. Test Virtual Server Isolation & Basic Routing
# -----------------------------------------------------------------------------
def test_port_1024():
    response = requests.get(f"http://{HOST}:1024/", timeout=TIMEOUT)
    assert response.status_code in [200, 403], f"Port 1024 returned unexpected status: {response.status_code}"

def test_port_1025_white_route():
    response = requests.get(f"http://{HOST}:1025/white/", timeout=TIMEOUT)
    assert response.status_code in [200, 403], f"Port 1025 returned unexpected status: {response.status_code}"

def test_port_1026_green_route():
    response = requests.get(f"http://{HOST}:1026/", timeout=TIMEOUT)
    assert response.status_code in [200, 403], f"Port 1026 returned unexpected status: {response.status_code}"

def test_404_not_found():
    response = requests.get(f"http://{HOST}:1024/this_path_does_not_exist/", timeout=TIMEOUT)
    assert response.status_code == 404, f"Expected 404 Not Found, got {response.status_code}"

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
# 3. Test HTTP Method Restrictions
# -----------------------------------------------------------------------------
def test_allowed_method_get():
    url = f"http://{HOST}:1025/white/"
    response = requests.get(url, timeout=TIMEOUT)
    assert response.status_code in [200, 403], f"Expected GET to be allowed, got {response.status_code}"

def test_disallowed_method_post():
    url = f"http://{HOST}:1025/white/"
    response = requests.post(url, data="test", timeout=TIMEOUT)
    assert response.status_code == 405, f"Expected 405 Method Not Allowed for POST, got {response.status_code}"

def test_disallowed_method_delete():
    url = f"http://{HOST}:1025/white/"
    response = requests.delete(url, timeout=TIMEOUT)
    assert response.status_code == 405, f"Expected 405 Method Not Allowed for DELETE, got {response.status_code}"

# -----------------------------------------------------------------------------
# 4. Test Redirects (Internal and External)
# -----------------------------------------------------------------------------
def test_internal_redirect():
    url = f"http://{HOST}:1025/green/"
    # allow_redirects=False is crucial here to catch the 3xx response before requests follows it
    response = requests.get(url, timeout=TIMEOUT, allow_redirects=False)
    
    assert response.status_code in [301, 302, 303, 307, 308], f"Expected a 3xx redirect status, got {response.status_code}"
    assert "Location" in response.headers, "Response is missing the 'Location' header"
    assert response.headers["Location"] == "/white/", f"Expected redirect to /white/, got {response.headers['Location']}"

def test_external_redirect():
    url = f"http://{HOST}:1025/external/"
    response = requests.get(url, timeout=TIMEOUT, allow_redirects=False)
    
    assert response.status_code in [301, 302, 303, 307, 308], f"Expected a 3xx redirect status, got {response.status_code}"
    assert "Location" in response.headers, "Response is missing the 'Location' header"
    assert response.headers["Location"] == "http://google.com", f"Expected redirect to http://google.com, got {response.headers['Location']}"

# -----------------------------------------------------------------------------
# 5. Test Autoindex (Directory Listing)
# -----------------------------------------------------------------------------
def test_autoindex_off():
    # Port 1024 does not have auto_index enabled
    url = f"http://{HOST}:1024/"
    response = requests.get(url, timeout=TIMEOUT)
    
    # Assuming no valid index file is present in root, it should forbid access or not found
    # (If an index.html IS present, this test will pass as 200, but shouldn't show a listing)
    assert "href=" not in response.text or response.status_code in [403, 404], "Expected autoindex to be OFF, but found directory listing HTML"

def test_autoindex_on():
    # Port 1027 has auto_index enabled globally
    url = f"http://{HOST}:1027/"
    response = requests.get(url, timeout=TIMEOUT)
    
    assert response.status_code == 200, f"Expected 200 OK for autoindex directory listing, got {response.status_code}"
    # A standard directory listing usually contains anchor tags referencing files
    assert "href=" in response.text, "Expected autoindex HTML output with links, but none were found"

# -----------------------------------------------------------------------------
# 6. Test CGI Interpreters
# -----------------------------------------------------------------------------
@pytest.mark.parametrize("script", ["test.py", "test.sh"])
def test_supported_cgi_scripts(script):
    url = f"http://{HOST}:1024/cgi-bin/{script}"
    response = requests.get(url, timeout=TIMEOUT)
    assert response.status_code == 200, f"Failed to execute supported CGI script {script}"

def test_missing_cgi_script():
    url = f"http://{HOST}:1024/cgi-bin/this_script_does_not_exist.py"
    response = requests.get(url, timeout=TIMEOUT)
    assert response.status_code == 404, f"Expected 404 Not Found for missing CGI script, got {response.status_code}"

# -----------------------------------------------------------------------------
# 7. Stress Testing (Siege)
# -----------------------------------------------------------------------------
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
    
    if os.path.exists(urls_file):
        os.remove(urls_file)
    
    output = result.stderr + result.stdout
    
    # Strip ALL whitespace (spaces, tabs, newlines) from the output to make the assertion bulletproof
    clean_output = re.sub(r'\s+', '', output)
    
    assert '"failed_transactions":0' in clean_output, \
        f"Siege test failed or dropped connections!\nOutput:\n{output}"
