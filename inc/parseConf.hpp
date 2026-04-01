#ifndef PARSE_CONF_HPP
# define PARSE_CONF_HPP

#include <string>
#include <vector>

void checkExtension(std::string fileName);
void checkForServerBlock(bool *isServerPresent, std::string line);

#endif