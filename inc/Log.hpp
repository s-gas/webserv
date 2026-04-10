#ifndef LOG_HPP
#define LOG_HPP

#include "defines.hpp"
#include <fstream>
#include <sstream>
#include <string>

class Log {

public:
  enum LogLevel { DEBUG, ERROR, INFO };

  Log(LogLevel);
  ~Log();

  static void setLogFile(const std::string &filename);

  template <typename T> Log &operator<<(const T &msg) {
    _ss << msg;
    return *this;
  }

private:
  LogLevel _level;
  std::stringstream _ss;

  static std::ofstream _fileStream;

  static void log(LogLevel level, const std::string &msg);

  // Rest of OFC
  Log();
  Log(Log const &);
  Log &operator=(Log const &);
};

#endif
