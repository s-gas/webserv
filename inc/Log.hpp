#ifndef LOG_HPP
#define LOG_HPP

#include <sstream>
#include <string>

#define RESET "\033[0m"
#define RED "\033[31m"
#define GREEN "\033[32m"
#define CYAN "\033[36m"

class Log {

public:
  enum LogLevel { DEBUG, INFO, ERROR };

  Log(LogLevel);

  static void debug(Log);
  static void info(Log);
  static void error(const std::string &msg);

  template <typename T> Log &operator<<(const T &msg) {
    _ss << msg;
    return *this;
  }

private:
  LogLevel _level;
  std::stringstream _ss;
  static void log(LogLevel level, const std::string &msg);

  Log();                       // empty constructor
  Log(Log const &);            // copy constructor
  Log &operator=(Log const &); // copy assignment operator
  ~Log();                      // destructor
};

#endif
