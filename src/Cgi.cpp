#include "Cgi.hpp"
#include <cstdlib>
#include <cstring>
#include <sys/stat.h>
#include <unistd.h>

// public ----------------------------------------------------------------------

Cgi::Cgi(HttpRequest &request, const std::string &sp)
    : scriptPath(sp), argArr(NULL) {
  initEnv(request);
  envArr = mapToArr(envMap);
}

Cgi::~Cgi() {
  if (envArr)
    freeArr(envArr);
  if (argArr)
    freeArr(argArr);
}

void Cgi::initCgi(char **&envp) {
  std::vector<std::string> envPath;
  setEnvPath(envp, envPath);
}

// private ---------------------------------------------------------------------

void Cgi::setEnvPath(char **&envp, std::vector<std::string> &envPath) {
  char *pathArr = NULL;
  char *path = NULL;
  size_t i = 0;
  while (envp[i] != NULL) {
    if (std::strncmp("PATH=", envp[i], 5) == 0) {
      pathArr = strdup(envp[i] + 5);
      break;
    }
    ++i;
  }
  if (!pathArr)
    throw std::runtime_error("CGI: Path Setup Failed");
  path = std::strtok(pathArr, ":");
  while (path != NULL) {
    envPath.push_back(std::string(path));
    path = std::strtok(NULL, ":");
  }
  free(pathArr);
}

// populating the env-Map
void Cgi::initEnv(HttpRequest &request) {
  std::string path = request.endpoint;
  std::string query = "";
  size_t questionMarkPos = path.find("?");
  if (questionMarkPos != std::string::npos) {
    query = path.substr(questionMarkPos + 1);
    path = path.substr(0, questionMarkPos);
  }
  envMap["GATEWAY_INTERFACE"] = "CGI/1.1";
  envMap["SERVER_PROTOCOL"] = "HTTP/1.1";
  envMap["REQUEST_METHOD"] = request.method;
  envMap["QUERY_STRING"] = query;
  envMap["PATH_INFO"] = path;
  envMap["CONTENT_TYPE"] = request.contentType;
  envMap["SCRIPT_FILENAME"] = scriptPath;

  std::stringstream ss;
  ss << request.contentLength;
  envMap["CONTENT_LENGTH"] = ss.str();
}

char **Cgi::mapToArr(std::map<std::string, std::string> &m) {
  char **arr = new char *[m.size() + 1];

  size_t i = 0;
  for (std::map<std::string, std::string>::const_iterator it = m.begin();
       it != m.end(); ++it) {
    std::string entry = it->first + "=" + it->second;
    arr[i] = new char[entry.size() + 1];
    std::strcpy(arr[i], entry.c_str());
    i++;
  }
  arr[i] = NULL;
  return arr;
}

void Cgi::freeArr(char **&arr) {
  if (!arr) {
    return;
  }
  for (size_t i = 0; arr[i] != NULL; ++i) {
    delete[] arr[i];
  }
  delete[] arr;
  arr = NULL;
}
