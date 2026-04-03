#ifndef LOG_HPP
#define LOG_HPP

#include "defines.hpp"
#include <sstream>
#include <string>

#define RESET "\033[0m"
#define RED "\033[31m"
#define GREEN "\033[32m"
#define CYAN "\033[36m"

class Log {

public:
  Log(status);
  ~Log();

  template <typename T> Log &operator<<(const T &msg) {
    _ss << msg;
    return *this;
  }

private:
  status _level;
  std::stringstream _ss;

  static void log(status level, const std::string &msg);

  // Rest of OFC
  Log();
  Log(Log const &);
  Log &operator=(Log const &);
};

#endif
