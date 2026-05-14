#include "Log.hpp"
#include <iostream>
#include <stdexcept>

std::ofstream Log::_fileStream;

// public ----------------------------------------------------------------------

Log::Log(LogLevel l) : level(l) {}

Log::~Log() {
  try {
    log(level, ss.str());
  } catch (...) {
  }
}

void Log::setLogFile(const std::string &filename) {
  if (_fileStream.is_open())
    _fileStream.close();
  _fileStream.open(filename.c_str(), std::ios::trunc);
  if (!_fileStream.is_open())
    throw std::runtime_error("Could not open log file");
}

// private ---------------------------------------------------------------------

void Log::log(LogLevel l, const std::string &msg) {
  std::string label;
  std::string color;
  std::ostream &outStream = (l == ERROR) ? std::cerr : std::cout;

  switch (l) {
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
