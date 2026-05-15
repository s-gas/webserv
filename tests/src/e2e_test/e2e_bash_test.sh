#!/bin/bash

# Colors for output
GREEN='\033[0;32m'
RED='\033[0;31m'
NC='\033[0m' # No Color

# Counter to track failed tests for CI/CD exit codes
FAIL_COUNT=0

echo "--- Starting Webserver Validation Tests ---"

# 1. Test Virtual Server Isolation (Ports 1024, 1025, 1026)
echo -e "\n${GREEN}Testing Port Isolation:${NC}"

curl -s -o /dev/null -w "%{http_code}" http://localhost:1024/ | grep "200\|403" > /dev/null \
    && echo -e "Port 1024: ${GREEN}OK${NC}" \
    || { echo -e "Port 1024: ${RED}FAILED${NC}"; FAIL_COUNT=$((FAIL_COUNT + 1)); }

curl -s -o /dev/null -w "%{http_code}" http://localhost:1025/white/ | grep "200\|403" > /dev/null \
    && echo -e "Port 1025: ${GREEN}OK${NC}" \
    || { echo -e "Port 1025: ${RED}FAILED${NC}"; FAIL_COUNT=$((FAIL_COUNT + 1)); }

curl -s -o /dev/null -w "%{http_code}" http://localhost:1026/ | grep "200\|403" > /dev/null \
    && echo -e "Port 1026 (Green): ${GREEN}OK${NC}" \
    || { echo -e "Port 1026: ${RED}FAILED${NC}"; FAIL_COUNT=$((FAIL_COUNT + 1)); }


# 2. Test Client Max Body Size (Limit: 20 bytes)
echo -e "\n${GREEN}Testing Body Size Limits on /submit/:${NC}"

echo -n "Small body (under 20)..."
curl -s -o /dev/null -w "%{http_code}" -X POST -d "hello" http://localhost:1024/submit/ | grep "200\|201\|204" > /dev/null \
    && echo -e " -> ${GREEN}PASS${NC}" \
    || { echo -e " -> ${RED}FAIL${NC}"; FAIL_COUNT=$((FAIL_COUNT + 1)); }

echo -n "Large body (over 20)..."
curl -s -o /dev/null -w "%{http_code}" -X POST -d "this_is_a_very_long_sentence_over_20_bytes" http://localhost:1024/submit/ | grep "413" > /dev/null \
    && echo -e " -> ${GREEN}PASS (Correctly rejected with 413)${NC}" \
    || { echo -e " -> ${RED}FAIL (Should have been 413)${NC}"; FAIL_COUNT=$((FAIL_COUNT + 1)); }


# 3. Test CGI Interpreters
echo -e "\n${GREEN}Testing Configured CGI Interpreters:${NC}"
scripts=("test.py" "test.sh")

for script in "${scripts[@]}"; do
    echo -n "Testing $script: "
    curl -s -o /dev/null -w "%{http_code}" "http://localhost:1024/cgi-bin/$script" | grep "200" > /dev/null \
        && echo -e "${GREEN}OK${NC}" \
        || { echo -e "${RED}FAILED${NC}"; FAIL_COUNT=$((FAIL_COUNT + 1)); }
done

echo -n "Testing unsupported script (test.php): "
curl -s -o /dev/null -w "%{http_code}" "http://localhost:1024/cgi-bin/test.php" | grep -v "200" > /dev/null \
    && echo -e "${GREEN}PASS (Correctly rejected/not executed)${NC}" \
    || { echo -e "${RED}FAIL (Returned 200 unexpectedly)${NC}"; FAIL_COUNT=$((FAIL_COUNT + 1)); }


# 4. Stress Test with Siege
echo -e "\n${GREEN}Starting 30-second Load Test with Siege...${NC}"

cat << EOF > .siege_urls.txt
http://localhost:1024/
http://localhost:1024/cgi-bin/test.py
http://localhost:1025/white/
http://localhost:1026/
EOF

# Run siege. If siege itself crashes or fails, catch it.
siege -c20 -t30S -f .siege_urls.txt || { echo -e "\n${RED}Siege test failed to execute properly.${NC}"; FAIL_COUNT=$((FAIL_COUNT + 1)); }

# Cleanup
rm -f .siege_urls.txt


# 5. Final Evaluation and Exit Code
echo -e "\n----------------------------------------"
if [ "$FAIL_COUNT" -ne 0 ]; then
    echo -e "${RED}❌ Tests finished with $FAIL_COUNT error(s).${NC}"
    exit 1
else
    echo -e "${GREEN}✅ All tests passed successfully!${NC}"
    exit 0
fi
