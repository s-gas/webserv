#!/usr/bin/python3
import sys

# Read all data from standard input (POST body)
input_data = sys.stdin.read()

# Convert the data to uppercase
upper_body = input_data.upper()

# If the input was empty, provide a default fallback message so you can see it works
if not upper_body:
    upper_body = "NO STDIN DATA RECEIVED OR BODY WAS EMPTY."

# Format the final HTML output
response_body = f"""<html>
<head><title>Webserv CGI UpperCase Test</title></head>
<body>
    <h1>CGI Uppercase Results</h1>
    <pre>{upper_body}</pre>
</body>
</html>"""

# Header
sys.stdout.write("Content-Type: text/html\r\n")
sys.stdout.write(f"Content-Length: {len(response_body.encode('utf-8'))}\r\n")
sys.stdout.write("\r\n")

# Body
sys.stdout.write(response_body)
sys.stdout.flush()
