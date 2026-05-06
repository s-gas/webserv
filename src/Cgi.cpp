#include "Cgi.hpp"
#include <cstring>

Cgi::Cgi(HttpRequest &request, const std::string &sP)
    : scriptPath(sP), args(NULL) {
  initEnv(request);
  envp = mapToArr(env);
}

Cgi::~Cgi() {
  if (envp)
    freeArr(envp);
  if (args)
    freeArr(args);
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
  env["GATEWAY_INTERFACE"] = "CGI/1.1";
  env["SERVER_PROTOCOL"] = "HTTP/1.1";
  env["REQUEST_METHOD"] = request.method;
  env["QUERY_STRING"] = query;
  env["PATH_INFO"] = path;
  env["CONTENT_TYPE"] = request.contentType;
  env["SCRIPT_FILENAME"] = scriptPath;

  std::stringstream ss;
  ss << request.contentLength;
  env["CONTENT_LENGTH"] = ss.str();
}

char** Cgi::mapToArr(std::map<std::string, std::string> &m) {
  char** arr = new char*[m.size() + 1];

  size_t i = 0;
  for (std::map<std::string, std::string>::const_iterator it = m.begin(); it != m.end(); ++it) {
    std::string entry = it->first + "=" + it->second;
    arr[i] = new char[entry.size() + 1];
    std::strcpy(arr[i], entry.c_str());
    i++;
  }
  arr[i] = NULL;
  return arr;
}

void Cgi::freeArr(char** &arr) {
  if (!arr) {
    return;
  }
  for (size_t i = 0; arr[i] != NULL; ++i) {
    delete[] arr[i];
  }
  delete[] arr;
  arr = NULL;
}
