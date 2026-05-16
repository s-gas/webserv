#include "integ_test.hpp"
#include "Http.hpp"
#include "Server.hpp"
#include "Cgi.hpp"
#include <sys/wait.h>
#include <assert.h>
#include <iostream>

void testCgiGet() {
  std::cout << "Test CGI-GET" << std::endl;
  createTestScript(
      "testGet.py",
      "import os\n"
      "print('Status: 200 OK')\n"
      "print('Content-Type: test/plain\\n')\n"
      "print('Method: ' + os.environ.get('REQUEST_METHOD', 'MISSING'))\n");

  // Mock Objects
  Location loc;
  loc.root = "./";
  loc.endpoint = "/";
  loc.cgi[".py"].push_back("/usr/bin/python3");

  HttpRequest req;
  req.method = "GET";
  req.version = "HTTP/1.1";
  req.file = "testGet.py";
  req.endpoint = "/";

  HttpResponse res;
  res.status = "200";

  // Setup and Execute

  Cgi cgi(res, req, loc);
  int readFd = -1;
  int writeFd = -1;

  cgi.execScript(readFd, writeFd);

  // Assert writeFd is closed for GET requests
  assert(writeFd == -1);
  assert(readFd != -1);

  // Wait for child process to finish
  int status;
  waitpid(cgi.childPid, &status, 0);

  // Read the result from the pipe
  char buffer[1024] = {0};
  ssize_t bytesRead = read(readFd, buffer, sizeof(buffer) -1);
  close(readFd);

  std::string output(buffer);
  assert(bytesRead > 0);
  assert(output.find("Method: GET") != std::string::npos);

  unlink("testGet.py"); // CleanUp
  std::cout << "PASS" << std::endl;
}
