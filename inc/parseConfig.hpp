#ifndef PARSE_HPP
# define PARSE_HPP

#include <fstream>
#include <string>
#include <vector>
#include <set>
#include "Server.hpp"

void readFile(std::ifstream &file, std::string &fileName);
void checkExtension(std::string &fileName);
void parseConfig(Block &block, std::ifstream &file);
void parseDirectives(Block &block, std::ifstream &file, int level, int &numBraces, bool &hasServer);
void parseDirective(std::string &line, Block &block);
std::string parseSingleValue(std::vector<std::string> tokens);
int parsePort(std::vector<std::string> tokens);
std::set<std::string> parseMultipleValues(std::vector<std::string> tokens);
void inheritDirectives(Block &child, Block &parent);
bool isClosing(std::string line);
bool isOpening(std::string line);
bool isBlock(std::string line, std::string keyword);
void storeEndPoint(Location &location, std::string line);
bool isNotEmpty(std::string line);
void checkOnlySpaces(std::string line, size_t start, size_t end);
void checkBetweenDirectiveAndBrace(std::string line, size_t start, size_t end, std::string type);
void checkEndPoint(std::string line, size_t start, size_t end);
std::vector<std::string> split(std::string line);
bool isNumber(std::string string);
std::string removeSemicolon(std::string token);

#endif
