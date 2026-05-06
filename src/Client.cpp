#include "Client.hpp"
#include "Log.hpp"
#include "Server.hpp"
#include "defines.hpp"
#include "readRequest.hpp"

Client::Client() : server(NULL) {}

Client::Client(Server &s, int clientFd) : server(&s), fd(clientFd) {}

void Client::handleData() {
  std::string requestString = readRequest(fd);
  std::cout << requestString << std::endl;
  // HttpRequest request(requestString);
  // HttpResponse response;
  request = requestString;
  response.generate(request);
  write(fd, response.response.c_str(), response.response.size());
  LOG_INFO << "Disconnecting client FD: " << fd;
  close(fd);
}
