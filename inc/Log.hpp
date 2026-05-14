#ifndef LOG_HPP
#define LOG_HPP

#include <fstream>
#include <sstream>
#include <string>
#include "defines.hpp"

class Log {

public:
  enum LogLevel { DEBUG, ERROR, INFO };

  Log(LogLevel);
  ~Log();

  static void setLogFile(const std::string &filename);

  template <typename T> Log &operator<<(const T &msg) {
    ss << msg;
    return *this;
  }

private:
  LogLevel level;
  std::stringstream ss;

  static std::ofstream _fileStream;

  static void log(LogLevel l, const std::string &msg);

  // Rest of OFC
  Log();
  Log(Log const &);
  Log &operator=(Log const &);
};

#endif
