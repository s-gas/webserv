#!/bin/bash

# Colors for output
GREEN='\033[0;32m'
RED='\033[0;31m'
NC='\033[0m' # No Color

echo "--- Starting Webserver Validation Tests ---"

# 1. Test Virtual Server Isolation (Ports 1024, 1025, 1026)
echo -e "\n${GREEN}Testing Port Isolation:${NC}"
curl -s -o /dev/null -w "%{http_code}" http://localhost:1024/ | grep "200" > /dev/null && echo "Port 1024: OK" || echo -e "${RED}Port 1024: FAILED${NC}"
curl -s -o /dev/null -w "%{http_code}" http://localhost:1025/white/ | grep "200" > /dev/null && echo "Port 1025: OK" || echo -e "${RED}Port 1025: FAILED${NC}"
curl -s -o /dev/null -w "%{http_code}" http://localhost:1026/ | grep "200" > /dev/null && echo "Port 1026 (Green): OK" || echo -e "${RED}Port 1026: FAILED${NC}"

# 2. Test Client Max Body Size (Limit: 20 bytes)
echo -e "\n${GREEN}Testing Body Size Limits on /submit/:${NC}"

echo "Small body (under 20)..."
curl -s -o /dev/null -w "%{http_code}" -X POST -d "hello" http://localhost:1024/submit/ | grep "201\|200" \
    && echo -e " -> ${GREEN}PASS${NC}" || echo -e " -> ${RED}FAIL${NC}"

echo "Large body (over 20)..."
curl -s -o /dev/null -w "%{http_code}" -X POST -d "this_is_a_very_long_sentence_over_20_bytes" http://localhost:1024/submit/ | grep "413" \
    && echo -e " -> ${GREEN}PASS (Correctly rejected with 413)${NC}" || echo -e " -> ${RED}FAIL (Should have been 413)${NC}"

# 3. Test CGI Interpreters
echo -e "\n${GREEN}Testing CGI Interpreters:${NC}"
scripts=("test.py" "test.sh" "test.pl" "test.rb" "test.js" "test.php")

for script in "${scripts[@]}"; do
    echo -n "Testing $script: "
    # Using GET instead of HEAD
    curl -s -o /dev/null -w "%{http_code}" "http://localhost:1024/cgi-bin/$script" | grep "200" > /dev/null \
        && echo -e "${GREEN}OK${NC}" || echo -e "${RED}FAILED${NC}"
done

# 4. Stress Test with Siege
echo -e "\n${GREEN}Starting 30-second Load Test with Siege...${NC}"

# Create a temporary URL file for Siege
cat << EOF > .siege_urls.txt
http://localhost:1024/index.html
http://localhost:1024/cgi-bin/test.py
http://localhost:1025/white/
http://localhost:1026/
EOF

siege -c20 -t30S -f .siege_urls.txt

# Cleanup
rm .siege_urls.txt
echo -e "\n${GREEN}Tests Complete.${NC}"
