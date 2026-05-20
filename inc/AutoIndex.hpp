#ifndef AUTOINDEX_HPP
#define AUTOINDEX_HPP

#include <string>

class AutoIndex {
public:
  std::string time;
  std::string name;
  std::string path;
  size_t size;
  bool isDir;

  void setInfo();
};

#endif
