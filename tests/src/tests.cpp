#include "tests.hpp"
#include "Block.hpp"

int main() {
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
    testReadFile(1, "conf-files/test1.conf", "pass");
    testReadFile(2, "conf-files/test2.conf", "pass");
    testReadFile(3, "conf-files/dontexist.conf", "throw");
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
}