#ifndef DEFINES_HPP
#define DEFINES_HPP

// Macros

#define RESET "\033[0m"
#define RED "\033[31m"
#define GREEN "\033[32m"
#define CYAN "\033[36m"

#define LOG_INFO_ON false
#define LOG_ERROR Log(Log::ERROR)
#define LOG_DEBUG Log(Log::DEBUG)
#define LOG_INFO if (!LOG_INFO_ON) {} else Log(Log::INFO)

#define MAX_EVENTS 32

// Constants
const int PORT = 8080;

// Enums
enum Status { SUCCESS = 0, FAILURE = 1, ERROR = -1 };

#endif
