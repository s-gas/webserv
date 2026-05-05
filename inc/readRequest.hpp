#ifndef READ_REQUEST_HPP
# define READ_REQUEST_HPP

# include <iostream>
# include <string>

std::string readRequest(int fd);
size_t getContentLength(std::string request);
bool isHeaderField(std::string headerFile, std::string line);

#endif
