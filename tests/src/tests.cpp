#include "tests.hpp"
#include "Server.hpp"

int main() {
    int failures = 0;

    std::cout << "\n=== Testing Config Parsing ===" << std::endl;
    // checkExtension()
    std::cout << "\n Testing checkExtension()" << std::endl;
    failures += testCheckExtension(1, "", "throw");
    failures += testCheckExtension(2, "wrong", "throw");
    failures += testCheckExtension(3, "text.txt", "throw");
    failures += testCheckExtension(4, "conf", "throw");
    failures += testCheckExtension(5, ".conf", "pass");
    failures += testCheckExtension(6, "nginx.conf", "pass");
    failures += testCheckExtension(7, "server.conf", "pass");
    failures += testCheckExtension(8, "something.conff", "throw");
    failures += testCheckExtension(9, "else.con", "throw");
    failures += testCheckExtension(10, "config", "throw");

    // readFile()
    std::cout << "\n Testing readFile()" << std::endl;
    failures += testReadFile(1, "conf-files/test1.conf", "pass");
    failures += testReadFile(2, "conf-files/test2.conf", "pass");
    failures += testReadFile(3, "conf-files/dontexist.conf", "throw");

    // parseDirectives()
    std::cout << "\n Testing parseDirectives()" << std::endl;
    failures += testParseDirectives(1, "conf-files/test1.conf", "pass");
    failures += testParseDirectives(2, "conf-files/test2.conf", "throw");
    failures += testParseDirectives(3, "conf-files/test3.conf", "throw");
    failures += testParseDirectives(4, "conf-files/test4.conf", "throw");
    failures += testParseDirectives(5, "conf-files/test5.conf", "throw");
    failures += testParseDirectives(6, "conf-files/test6.conf", "throw");
    failures += testParseDirectives(7, "conf-files/test7.conf", "throw");
    failures += testParseDirectives(8, "conf-files/test8.conf", "pass");
    failures += testParseDirectives(9, "conf-files/test9.conf", "throw");
    failures += testParseDirectives(10, "conf-files/test10.conf", "pass");
    failures += testParseDirectives(11, "conf-files/test11.conf", "pass");
    failures += testParseDirectives(12, "conf-files/test12.conf", "pass");
    std::string endpoints[10];
    endpoints[0] = "/";
    failures += testEndpoints(13, "conf-files/test10.conf", endpoints, 1, 0);
    endpoints[0] = "/api";
    failures += testEndpoints(14, "conf-files/test11.conf", endpoints, 1, 0);
    endpoints[0] = "/var/www/";
    failures += testEndpoints(15, "conf-files/test12.conf", endpoints, 1, 0);
    endpoints[0] = "/var/www/";
    endpoints[1] = "/";
    endpoints[2] = "/api";
    failures += testEndpoints(16, "conf-files/test13.conf", endpoints, 3, 0);
    failures += testParseDirectives(17, "conf-files/test15.conf", "throw");
    failures += testParseDirectives(18, "conf-files/test16.conf", "throw");
    failures += testParseDirectives(19, "conf-files/test17.conf", "throw");

    // multiple ports
    std::cout << "\n Testing multiple ports" << std::endl;
    failures += testListenDirective(20, "conf-files/test14.conf", 8000);
    failures += testListenDirective(21, "conf-files/test14.conf", 8080);
    failures += testListenDirective(22, "conf-files/test14.conf", 8001);
    failures += testListenDirective(23, "conf-files/test14.conf", 6060);
    failures += testListenDirective(24, "conf-files/test14.conf", 420);

    // full parse
    std::cout << "\n Testing full parsing" << std::endl;
    failures += testParse(1, "conf-files/test18.conf", "throw");
    failures += testParse(2, "conf-files/test19.conf", "throw");
    failures += testParse(3, "conf-files/test20.conf", "throw");
    failures += testParse(4, "conf-files/test21.conf", "throw");

    // test22.conf: Multi-Server & Multi-Port Validation
    std::cout << "\n Multi-Server & Multi-Port" << std::endl;
    failures += testParse(1, "conf-files/test22.conf", "pass");
    failures += testListenDirective(2, "conf-files/test22.conf", 80);
    failures += testListenDirective(3, "conf-files/test22.conf", 8080);
    failures += testListenDirective(4, "conf-files/test22.conf", 443);
    std::string s0_endpoints[2];
    std::string s1_endpoints[2];
    s0_endpoints[0] = "/";      // Server 1
    s0_endpoints[1] = "/api";   // Server 1
    s1_endpoints[0] = "/";      // Server 2
    s1_endpoints[1] = "/assets";// Server 2
    failures += testEndpoints(5, "conf-files/test22.conf", s0_endpoints, 2, 0);
    failures += testEndpoints(6, "conf-files/test22.conf", s1_endpoints, 2, 1);

    // getContentLength()
    std::cout << "\nTesting getContentLength()" << std::endl;
    failures += testGetContentLength(1, "Content-Length: 0", 0);
    failures += testGetContentLength(2, "random", 0);
    failures += testGetContentLength(3, "Content-Lengt: 0", 0);
    failures += testGetContentLength(4, "Content-Length: 42", 42);

    std::cout << "\n=== Testing HttpRequest Parsing ===" << std::endl;

    // parseRequest()
    std::string str = "GET / HTTP/1.1\r\n"
                      "User-Agent: PostmanRuntime/7.53.0\r\n"
                      "Accept: */*\r\n"
                      "Postman-Token: a602d5cf-1f49-4ada-9660-85aba4d799f8\r\n"
                      "Host: localhost:8080\r\n"
                      "Accept-Encoding: gzip, deflate, br\r\n"
                      "Connection: keep-alive\r\n"
                      "\r\n";
    HttpRequest request;
    request.method = "GET";
    request.version = "HTTP/1.1";
    request.contentType = "";
    request.contentLength = 0;
    request.body = "";
    failures += testHttp(1, str, request);
    request.method = "POST";
    std::string str2 = "POST / HTTP/1.1\r\n"
                      "User-Agent: PostmanRuntime/7.53.0\r\n"
                      "Accept: */*\r\n"
                      "Postman-Token: a602d5cf-1f49-4ada-9660-85aba4d799f8\r\n"
                      "Host: localhost:8080\r\n"
                      "Accept-Encoding: gzip, deflate, br\r\n"
                      "Connection: keep-alive\r\n"
                      "\r\n";
    failures += testHttp(2, str2, request);
    std::string str3 =  "POST / HTTP/1.1\r\n"
                        "Content-Type: application/json\r\n"
                        "User-Agent: PostmanRuntime/7.53.0\r\n"
                        "Accept: */*\r\n"
                        "Content-Length: 14\r\n"
                        "\r\n"
                        "hello stranger";
    request.method = "POST";
    request.version = "HTTP/1.1";
    request.contentType = "application/json";
    request.body = "hello stranger";
    request.contentLength = 14;
    failures += testHttp(3, str3, request);
    std::string str4 =  "GET / HTTP/1.1\r\n"
                        "Content-Type: application/json\r\n"
                        "User-Agent: PostmanRuntime/7.53.0\r\n"
                        "Accept: */*\r\n"
                        "Content-Length: 14\r\n"
                        "\r\n"
                        "hello stranger\r\n"
                        "how are you?";
    request.method = "GET";
    request.contentLength = 27;
    request.body =  "hello stranger\n"
                    "how are you?";
    failures += testHttp(4, str4, request);

    // isHeaderField()
    failures += testIsHeaderField(1, "content-length: ", "Content-Length: 50", true);
    failures += testIsHeaderField(2, "content-lengt: ", "Content-Length: 50", false);
    failures += testIsHeaderField(3, "content-LENGTH: ", "CONTENT-Length: 50", true);
    failures += testIsHeaderField(4, "content-type: ", "CONTENT-TYPE: application/json", true);
    failures += testIsHeaderField(5, "CONTENT-TYPE: ", "content-TYPE: application/json", true);
    failures += testIsHeaderField(6, "CONTENT-TYPE= ", "content-TYPE: application/json", false);

    if (failures > 0) {
      std::cout << "\n\nTOTAL FAILURES: " << failures << std::endl;
      return FAILURE;
    } else {
      std::cout << "\n\nALL TESTS PASSED!" << std::endl;
      return SUCCESS;
    }
}
