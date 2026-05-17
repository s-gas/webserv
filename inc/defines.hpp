#ifndef DEFINES_HPP
#define DEFINES_HPP

#include <iostream>

// Macros

#define RESET "\033[0m"
#define RED "\033[31m"
#define GREEN "\033[32m"
#define CYAN "\033[36m"

// Makefile override check of LOG_INFO_ON
#ifndef LOG_INFO_ON
#define LOG_INFO_ON true
#endif

#define LOG_ERROR Log(Log::ERROR)
#define LOG_DEBUG Log(Log::DEBUG)
#define LOG_INFO if (!LOG_INFO_ON) {} else Log(Log::INFO)
#define LOG_REQUEST if (!LOG_INFO_ON) {} else std::cout << "REQUEST:\n" << requestRaw << std::endl;
#define LOG_RESPONSE if (!LOG_INFO_ON) {} else std::cout << "RESPONSE:\n" << responseRaw << std::endl;

#define MAX_EVENTS 1024

// Constants
const int PORT = 8080;

// Enums
enum Status { SUCCESS = 0, FAILURE = 1, ERROR = -1 };

enum ClientState {
  READING, // reading client request
  WRITING, // writing to CGI pipe
  PROCESSING, // waiting for CGI output
  SENDING, // sending response to client
  DONE
};

#endif
