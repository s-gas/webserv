#include "Cgi.hpp"
#include "Log.hpp"
#include "Server.hpp"
#include "defines.hpp"
#include "parseConfig.hpp"
#include "signal.hpp"
#include <iostream>

int main(int argc, char *argv[]) {
  std::string fileName;
  if (argc == 1) {
    fileName = "conf-files/default.conf";
  } else if (argc == 2) {
    fileName = argv[1];
  } else {
    std::cerr << "Usage: ./webserv <config file>" << std::endl;
    return 1;
  }
  setSignals();
  std::ifstream configFile;
  Config main;
  try {
    checkExtension(fileName);
    readFile(configFile, fileName);
    parseConfig(main, configFile);
    if (main.init() == SUCCESS) {
      main.run();
    }
  } catch (const std::exception &e) {
    LOG_ERROR << e.what();
    return FAILURE;
  }
  return SUCCESS;
}
