#ifndef CGI_HPP
#define CGI_HPP

#include "Http.hpp"
#include <map>
#include <string>
#include <vector>

class Cgi {

public:
  Cgi(HttpRequest &request, const std::string &sp);
  ~Cgi();

  std::string execute();

  static void initCgi(char **&env);

private:
  std::map<std::string, std::string> envMap;
  std::string scriptPath;
  char **envArr;
  char **argArr;

  void initEnv(HttpRequest &request);
  char **mapToArr(std::map<std::string, std::string> &m);
  void freeArr(char **&arr);
  static void closePipes(int *&pipe1, int *&pipe2);

  // part of Interpreters
  static std::map<std::string, std::string> interpreters;
  static void setEnvPath(char **&envp, std::vector<std::string> &envPath);
  static void setInterpreters(std::vector<std::string> &envPath);
  static bool pathIsValid(const std::string &path);
};

#endif
