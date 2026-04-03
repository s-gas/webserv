#include "webserv.hpp"
#include <iostream>

int main() {
  Server server(PORT);
  try {
    server.init();
    server.run();
  } catch (const std::exception &e) {
    LOG_ERROR << e.what();
    return FAILURE;
  }
  return SUCCESS;
}
