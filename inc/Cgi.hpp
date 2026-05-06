#ifndef CGI_HPP
#define CGI_HPP

#include "Http.hpp"
#include <string>
#include <map>

class Cgi {

public:
  Cgi(HttpRequest &request, const std::string &scriptPath);
  ~Cgi();
  std::string execute();

private:
  std::map<std::string, std::string> env;
  std::string scriptPath;
  char **envp;
  char **args;

  void initEnv(HttpRequest &request);
  char** mapToArr(std::map<std::string, std::string> &m);
  void freeArr(char** &arr);
};

#endif
