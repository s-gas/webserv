#ifndef CGI_HPP
#define CGI_HPP

#include "Http.hpp"
#include "Server.hpp"
#include <map>
#include <string>

class Cgi {

public:
  Cgi(HttpRequest &request, Location &location);
  ~Cgi();

  std::string execute();

private:
  std::string root;
  std::map<std::string, std::string> interpreter;
  std::map<std::string, std::string> envMap;
  std::string scriptName;  // path including .file
  std::string pathInfo;    // trailing path after .file
  std::string queryString; // everything after ?
  char **envArr;
  char **argArr;

  void initEnv(HttpRequest &request);
  void initArgs(const std::string &interpreterPath);
  char **mapToArr(std::map<std::string, std::string> &m);
  void freeArr(char **&arr);
  void freeArg();
  static void closePipes(int *pipe1, int *pipe2);

  Cgi(Cgi const &);
  Cgi &operator=(Cgi const &);
};

#endif
