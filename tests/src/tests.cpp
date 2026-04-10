#include "tests.hpp"
#include "Server.hpp"

int main() {
    int failures = 0;
    // checkExtension()
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
    failures += testReadFile(1, "conf-files/test1.conf", "pass");
    failures += testReadFile(2, "conf-files/test2.conf", "pass");
    failures += testReadFile(3, "conf-files/dontexist.conf", "throw");

    // parseDirectives()
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
    failures += testEndpoints(13, "conf-files/test10.conf", endpoints, 1);
    endpoints[0] = "/api";
    failures += testEndpoints(14, "conf-files/test11.conf", endpoints, 1);
    endpoints[0] = "/var/www/";
    failures += testEndpoints(15, "conf-files/test12.conf", endpoints, 1);
    endpoints[0] = "/var/www/";
    endpoints[1] = "/";
    endpoints[2] = "/api";
    failures += testEndpoints(16, "conf-files/test13.conf", endpoints, 3);
    failures += testListenDirective(17, "conf-files/test14.conf", 8000);
    failures += testParseDirectives(18, "conf-files/test15.conf", "throw");
    failures += testParseDirectives(19, "conf-files/test16.conf", "throw");
    failures += testParseDirectives(20, "conf-files/test17.conf", "throw");
    failures += testListenDirective(21, "conf-files/test13.conf", 8080);
    failures += testParse(1, "conf-files/test18.conf", "throw");
    failures += testParse(2, "conf-files/test19.conf", "throw");
    failures += testParse(3, "conf-files/test20.conf", "throw");
    failures += testParse(4, "conf-files/test21.conf", "throw");
    if (failures > 0)
      return FAILURE;
    else
      return SUCCESS;
}
