#include "Log.hpp"
#include <iostream>
#include <stdexcept>

std::ofstream Log::_fileStream;

// public ----------------------------------------------------------------------

Log::Log(logLevel level) : _level(level) {}

Log::~Log() {
  try {
    log(_level, _ss.str());
  } catch (...) {
  }
}

void Log::setLogFile(const std::string &filename) {
  if (_fileStream.is_open())
    _fileStream.close();
  _fileStream.open(filename.c_str(), std::ios::app);
  if (!_fileStream.is_open())
    throw std::runtime_error("Could not open log file");
}

// private ---------------------------------------------------------------------


void Log::log(logLevel level, const std::string &msg) {
  std::string label;
  std::string color;
  std::ostream &outStream = (level == ERROR) ? std::cerr : std::cout;

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

  outStream << color << label << msg << RESET << std::endl;
  if (_fileStream.is_open()) {
    _fileStream << label << msg << std::endl;
  }
}
