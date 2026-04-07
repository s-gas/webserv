#include "tests.hpp"
#include "Block.hpp"

int main() {
    // checkExtension()
    testCheckExtension(1, "", "throw");
    testCheckExtension(2, "wrong", "throw");
    testCheckExtension(3, "text.txt", "throw");
    testCheckExtension(4, "conf", "throw");
    testCheckExtension(5, ".conf", "pass");
    testCheckExtension(6, "nginx.conf", "pass");
    testCheckExtension(7, "server.conf", "pass");
    testCheckExtension(8, "something.conff", "throw");
    testCheckExtension(9, "else.con", "throw");
    testCheckExtension(10, "config", "throw");

    // readFile()
    testReadFile(1, "conf-files/test1.conf", "pass");
    testReadFile(2, "conf-files/test2.conf", "pass");
    testReadFile(3, "conf-files/dontexist.conf", "throw");

    // parseDirectives()
    testParseDirectives(1, "conf-files/test1.conf", "pass");
    testParseDirectives(2, "conf-files/test2.conf", "throw");
    testParseDirectives(3, "conf-files/test3.conf", "throw");
    testParseDirectives(4, "conf-files/test4.conf", "throw");
    testParseDirectives(5, "conf-files/test5.conf", "throw");
    testParseDirectives(6, "conf-files/test6.conf", "throw");
    testParseDirectives(7, "conf-files/test7.conf", "throw");
    testParseDirectives(8, "conf-files/test8.conf", "pass");
    testParseDirectives(9, "conf-files/test9.conf", "throw");
    testParseDirectives(10, "conf-files/test10.conf", "pass");
    testParseDirectives(11, "conf-files/test11.conf", "pass");
    testParseDirectives(12, "conf-files/test12.conf", "pass");
    std::string endpoints[10];
    endpoints[0] = "/";
    testEndpoints(13, "conf-files/test10.conf", endpoints, 1);
    endpoints[0] = "/api";
    testEndpoints(14, "conf-files/test11.conf", endpoints, 1);
    endpoints[0] = "/var/www/";
    testEndpoints(15, "conf-files/test12.conf", endpoints, 1);
    endpoints[0] = "/var/www/";
    endpoints[1] = "/";
    endpoints[2] = "/api";
    testEndpoints(16, "conf-files/test13.conf", endpoints, 3);
    testListenDirective(17, "conf-files/test14.conf", 43);
    testParseDirectives(18, "conf-files/test15.conf", "throw");
    testParseDirectives(19, "conf-files/test16.conf", "throw");
    testParseDirectives(20, "conf-files/test17.conf", "throw");
    testListenDirective(21, "conf-files/test13.conf", 8080);
}