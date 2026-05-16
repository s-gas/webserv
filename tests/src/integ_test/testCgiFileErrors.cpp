#include "Cgi.hpp"
#include "Http.hpp"
#include "Server.hpp"
#include "integ_test.hpp"
#include <assert.h>
#include <iostream>
#include <sys/stat.h>
#include <sys/wait.h>

void testCgiFileErrors() {
  std::cout << "Test CGI-File-Errors" << std::endl;

  // Mock Objects
  Location loc;
  loc.root = "./";
  loc.endpoint = "/";
  loc.cgi[".py"].push_back("/usr/bin/python3");

  HttpRequest req;
  req.method = "GET";
  req.version = "HTTP/1.1";
  req.endpoint = "/";

  HttpResponse res;
  res.status = "200";

  // 1. Test 404 Not Found
  req.file = "doesNotExist.py";
  try {
    Cgi cgi(res, req, loc);
    int readFd = -1, writeFd = -1;
    cgi.execScript(readFd, writeFd);
    assert(false); // Should not reach here
  } catch (const std::exception &e) {
    assert(res.status == "404");
  }

  // 2. Test 403 Forbidden
  createTestScript("forbidden.py", "print('hello')");
  chmod("forbidden.py", 0000);

  req.file = "forbidden.py";
  res.status = "200";
  try {
    Cgi cgi(res, req, loc);
    int readFd = -1, writeFd = -1;
    cgi.execScript(readFd, writeFd);
    assert(false);
  } catch (const std::exception &e) {
    assert(res.status == "403");
  }

  unlink("forbidden.py");
  std::cout << "PASS" << std::endl;
}
