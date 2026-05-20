#include "integ_test.hpp"
#include "Http.hpp"
#include "Server.hpp"
#include "Cgi.hpp"
#include <sys/wait.h>
#include <assert.h>
#include <iostream>

void testCgiPost() {
  std::cout << "Test CGI-POST" << std::endl;

  createTestScript("testPost.py", "import sys\n"
                                  "body = sys.stdin.read()\n"
                                  "print('Echo: ' + body)\n");

  // Mock Objects
  Location loc;
  loc.root = "./";
  loc.endpoint = "/";
  loc.cgi[".py"].push_back("/usr/bin/python3");

  HttpRequest req;
  req.method = "POST";
  req.version = "HTTP/1.1";
  req.file = "testPost.py";
  req.endpoint = "testPost.py";
  req.contentLength = 13;

  HttpResponse res;
  res.status = "200";

  // Setup and Execute

  Cgi cgi(res, req, loc);
  int readFd = -1;
  int writeFd = -1;

  cgi.execScript(readFd, writeFd);

  // Assert writeFd is closed for GET requests
  assert(writeFd != -1);
  assert(readFd != -1);

  // Moch Body
  std::string body = "Hello Webserv";
  write(writeFd, body.c_str(), body.size());
  close(writeFd);

  // Wait for child process to finish
  int status;
  waitpid(cgi.childPid, &status, 0);

  // Read the result from the pipe
  char buffer[1024] = {0};
  ssize_t bytesRead = read(readFd, buffer, sizeof(buffer) -1);
  close(readFd);

  std::string output(buffer);
  assert(bytesRead > 0);
  assert(output.find("Echo: Hello Webserv") != std::string::npos);

  unlink("testPost.py"); // CleanUp
  std::cout << "PASS" << std::endl;
}
