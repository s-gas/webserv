#ifndef CGI_HPP
#define CGI_HPP

#include "Http.hpp"
#include "Server.hpp"
#include <map>
#include <string>

class Cgi {

public:
  pid_t childPid;
  Cgi(HttpResponse &response, HttpRequest &request, Location &location);
  ~Cgi();

  int execScript();

private:
  std::string root;
  std::map<std::string, std::vector<std::string> > interpreter;
  std::map<std::string, std::string> envMap;
  std::string scriptName;
  std::string scriptFileName;
  std::string &status;
  char **envArr;
  char **argArr;

  void checkScriptFileName();
  void setScriptFileName(HttpRequest &request, Location &location);
  void setEnvArr(HttpRequest &request);
  void setArgArr();
  char **mapToArr(std::map<std::string, std::string> &m);
  char **vectorToArr(std::vector<std::string> &v);
  static void freeArr(char **&arr);

  Cgi(Cgi const &);
  Cgi &operator=(Cgi const &);
};

#endif
