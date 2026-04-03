#include "Log.hpp"
#include <iostream>

// public ----------------------------------------------------------------------

Log::Log(status level) : _level(level) {}

Log::~Log() { log(_level, _ss.str()); }

// private ---------------------------------------------------------------------

void Log::log(status level, const std::string &msg) {
  std::string label;
  std::string color;

  switch (level) {
  case DEBUG:
    label = "[DEBUG] ";
    color = CYAN;
    break;
  case INFO:
    label = "[INFO] ";
    color = GREEN;
    break;
  case ERROR:
    label = "[ERROR] ";
    color = RED;
    break;
  }
  std::cout << color << label << msg << RESET << std::endl;
}
