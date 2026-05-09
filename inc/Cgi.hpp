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

  static std::string root;
  static std::map<std::string, std::string> interpreter;
  static void setCgiRoot(std::string cgiR);
  static void setCgiInterpreter(std::string key, std::string value);

private:
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
};

#endif
