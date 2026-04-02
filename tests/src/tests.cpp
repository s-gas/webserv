#include "tests.hpp"

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
    testCheckDirectiveBlocks(1, "conf-files/test1.conf", "pass");
    testCheckDirectiveBlocks(2, "conf-files/test2.conf", "throw");
    testCheckDirectiveBlocks(3, "conf-files/test3.conf", "throw");
    testCheckDirectiveBlocks(4, "conf-files/test4.conf", "pass");
    testCheckDirectiveBlocks(5, "conf-files/test5.conf", "throw");
}