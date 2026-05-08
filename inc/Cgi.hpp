#ifndef CGI_HPP
#define CGI_HPP

#include "Http.hpp"
#include <map>
#include <string>

class Cgi {

public:
  Cgi(HttpRequest &request, const std::string &sp);
  ~Cgi();

  std::string execute();

private:
  std::map<std::string, std::string> envMap;
  std::string scriptPath;
  char **envArr;
  char **argArr;

  void initEnv(HttpRequest &request);
  void initArgs(const std::string &interpreterPath);
  char **mapToArr(std::map<std::string, std::string> &m);
  void freeArr(char **&arr);
  void freeArg();
  static void closePipes(int *pipe1, int *pipe2);
};

#endif
