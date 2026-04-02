#ifndef TESTS_HPP
# define TESTS_HPP

#include <iostream>
#include <fstream>

#define RED     "\033[31m"
#define GREEN   "\033[32m"
#define RESET   "\033[0m"

void testCheckExtension(int N, std::string argument, std::string string);
void testParseDirectives(int N, std::string argument, std::string string);
void testReadFile(int N, std::string argument, std::string string);

#endif