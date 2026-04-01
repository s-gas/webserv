#include "webserv.hpp"
#include <iostream>

int main() {
  Server server(PORT);
  try {
    server.init();
    server.run();
  } catch (std::exception &e) {
    std::cerr << "[ERROR] " << e.what() << std::endl;
  }
}
