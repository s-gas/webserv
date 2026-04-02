#include "webserv.hpp"

int main() {
  Server server(PORT);
  try {
    server.init();
    server.run();
  } catch (const std::exception &e) {
    std::cerr << "[ERROR] " << e.what() << std::endl;
    return 1;
  }
  return 0;
}
