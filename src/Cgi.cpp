#include "Cgi.hpp"
#include <cstdlib>
#include <cstring>
#include <sys/stat.h>
#include <unistd.h>

// public ----------------------------------------------------------------------

Cgi::Cgi(HttpRequest &request, const std::string &sp)
    : scriptPath(sp), argArr(NULL), envArr(NULL) {
  initEnv(request);
  envArr = mapToArr(envMap);
}

Cgi::~Cgi() {
  if (envArr)
    freeArr(envArr);
  if (argArr)
    freeArr(argArr);
}

std::string Cgi::execute() {
  // Piping
  int pipePToC[] = {-1, -1}; // Parent -> Child
  int pipeCToP[] = {-1, -1}; // Child -> Parent

  if (pipe(pipePToC) == -1) {
    throw std::runtime_error("Cgi-Piping failed");
  }
  if (pipe(pipeCToP) == -1) {
    closePipes(pipePToC, pipeCToP);
    throw std::runtime_error("Cgi-Piping failed");
  }

  // Forking
  pid_t pid = fork();
  if (pid == -1) {
    closePipes(pipePToC, pipeCToP);
    throw std::runtime_error("Cgi-Forking failed");
  }

  // Child
  if (pid == 0) {
    if (dup2(pipePToC[0], STDIN_FILENO) == -1 ||
        dup2(pipeCToP[1], STDOUT_FILENO) == -1) {
      exit(1);
    }
    closePipes(pipePToC, pipeCToP);
  }
}

// private ---------------------------------------------------------------------

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

void Cgi::initArgs(const std::string &interpreterPath) {
  argArr = static_cast<char **>(malloc(sizeof(char*) * 3));
  argArr[0] = strdup(interpreterPath.c_str());
  if (!argArr[0])
    throw std::runtime_error("Malloc failed");
  argArr[1] = strdup(scriptPath.c_str());
  if (!argArr[1]) {
    free(argArr[0]);
    throw std::runtime_error("Malloc failed");
  }

  argArr[2] = NULL;
}

void Cgi::freeArgs() {
  if (argArr) {
    for (size_t i = 0; argArr[i]; ++i) {
      free(argArr[i]);
    }
    free(argArr);
  }
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

void Cgi::closePipes(int *pipe1, int *pipe2) {
  for (size_t i = 0; i < 2; ++i) {
    if (pipe1[i] != -1)
      close(pipe1[i]);
    if (pipe2[i] != -1)
      close(pipe2[i]);
  }
}
