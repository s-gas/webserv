#ifndef PARSE_HPP
# define PARSE_HPP

#include <fstream>
#include <string>

void readFile(std::ifstream &file, std::string &fileName);
void checkExtension(std::string &fileName);
void checkDirectiveBlocks(std::ifstream &file);

#endif