#include "Log.hpp"
#include "Server.hpp"
#include "defines.hpp"
#include "parse.hpp"
#include "signal.hpp"
#include <iostream>

int main(int argc, char *argv[]) {
  if (argc != 2) {
    std::cerr << "Usage: ./webserv <config file>" << std::endl;
    return 1;
  }
  setSignals();
  std::ifstream configFile;
  std::string fileName = argv[1];
  Config main;
  try {
    checkExtension(fileName);
    readFile(configFile, fileName);
    parse(main, configFile);
    main.server.init();
    main.run();
  } catch (const std::exception &e) {
    if (SignalState::serverRunning == 1) {
      LOG_ERROR << e.what();
      return FAILURE;
    }
  }
  return SUCCESS;
}
