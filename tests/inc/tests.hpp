#ifndef TESTS_HPP
# define TESTS_HPP

#include "Http.hpp"
#include <iostream>
#include <fstream>

#define RED     "\033[31m"
#define GREEN   "\033[32m"
#define RESET   "\033[0m"

int testCheckExtension(int N, std::string argument, std::string string);
int testReadFile(int N, std::string argument, std::string string);
int testParse(int N, std::string argument, std::string string);
int testParseDirectives(int N, std::string argument, std::string string);
int testEndpoints(int N, std::string argument, std::string *endpoints, size_t len);
int testListenDirective(int N, std::string argument, size_t port);
int testGetContentLength(int N, std::string argument, size_t result);
int testHttp(int N, std::string argument, HttpRequest request);

#endif
