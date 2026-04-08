#ifndef PARSE_HPP
# define PARSE_HPP

#include <fstream>
#include <string>
#include <vector>
#include "Server.hpp"

void readFile(std::ifstream &file, std::string &fileName);
void checkExtension(std::string &fileName);
void parse(Block &block, std::ifstream &file);
void parseDirectives(Block &block, std::ifstream &file, int level, int &numBraces);
bool isNotEmpty(std::string line);
void checkOnlySpaces(std::string line, size_t start, size_t end);
void checkBetweenDirectiveAndBrace(std::string line, size_t start, size_t end, std::string type);
void checkEndPoint(std::string line, size_t start, size_t end);
std::vector<std::string> split(std::string line);
bool isNumber(std::string string);
std::string removeSemicolon(std::string token);

#endif