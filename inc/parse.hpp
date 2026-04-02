#ifndef PARSE_HPP
# define PARSE_HPP

#include <fstream>
#include <string>
#include "Block.hpp"

void readFile(std::ifstream &file, std::string &fileName);
void checkExtension(std::string &fileName);
void parseDirectives(Block &block, std::ifstream &file, int level);

#endif