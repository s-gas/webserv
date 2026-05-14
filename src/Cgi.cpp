#include "Cgi.hpp"
#include "Client.hpp"
#include "Log.hpp"
#include <cstdlib>
#include <cstring>
#include <sys/stat.h>
#include <sys/wait.h>
#include <unistd.h>

// public ----------------------------------------------------------------------

/* next to-dos:
 * - set all variables within the constructor
 * - setting root and interpreters during config parsing
 * - build execute within the epoll, implement timeout
 */

Cgi::Cgi(HttpResponse &response, HttpRequest &request, Location &location) : status(response.status) {
  envArr = NULL;
  argArr = NULL;
  root = location.root;
  interpreter = location.cgi;
  // scriptName = request.file.empty() ? location.index : request.file;
  scriptName = request.file;
  setScriptFileName(request, location);
  setEnvArr(request);
  setArgArr();
}

Cgi::~Cgi() {
  if (envArr)
    freeArr(envArr);
  if (argArr)
    freeArr(argArr);
}

std::string Cgi::execute() {
  checkScriptFileName();
  envArr = mapToArr(envMap);
  // Piping
  int pipePToC[] = {-1, -1}; // Parent -> Child
  int pipeCToP[] = {-1, -1}; // Child -> Parent
  if (pipe(pipePToC) == -1 || pipe(pipeCToP) == -1) {
    closePipes(pipePToC, pipeCToP);
    status = "500";
    throw std::runtime_error("CGI: Piping failed");
  }

  // Forking
  pid_t pid = fork();
  if (pid == -1) {
    closePipes(pipePToC, pipeCToP);
    status = "500";
    throw std::runtime_error("CGI: Forking failed");
  }

  // Child
  if (pid == 0) {
    if (dup2(pipePToC[0], STDIN_FILENO) == -1 ||
        dup2(pipeCToP[1], STDOUT_FILENO) == -1) {
      exit(1);
    }
    closePipes(pipePToC, pipeCToP);
    if (execve(argArr[0], argArr, envArr) == -1)
      exit(1);
  }

  // Parent
  close(pipePToC[0]);
  close(pipeCToP[1]);
  if (!envMap["REQUEST_METHOD"].empty() && envMap["REQUEST_METHOD"] == "POST") {
    //    write(pipePToC[1], request.body.c_str(), request.body.size());
  }
  close(pipePToC[1]);

  char buffer[4096];
  std::string response;
  ssize_t bytesRead;
  while ((bytesRead = read(pipeCToP[0], buffer, sizeof(buffer))) > 0) {
    response.append(buffer, bytesRead);
  }
  close(pipeCToP[0]);
  waitpid(pid, NULL, 0);
  return response;
}

// private ---------------------------------------------------------------------

void Cgi::checkScriptFileName() {
  LOG_INFO << "scripteFileName = " << scriptFileName;
  if (access(scriptFileName.c_str(), F_OK) == -1) {
    status = "404";
    throw std::runtime_error("CGI: Script not found: " + scriptFileName);
  }

  if (access(scriptFileName.c_str(), R_OK) == -1) {
    status = "403";
    throw std::runtime_error("CGI: Script not found: " + scriptFileName);
  }

  struct stat st;
  if (stat(scriptFileName.c_str(), &st) == 0 && S_ISDIR(st.st_mode)) {
    status = "403";
    throw std::runtime_error("CGI: Requested script is a directory");
  }
}

void Cgi::setScriptFileName(HttpRequest &request, Location &location) {
  scriptFileName = location.root;
  scriptFileName +=
      location.endpoint == "/" ? request.endpoint : location.endpoint;
  scriptFileName += request.file.empty() ? location.index : request.file;

}

// populating the env-Map
void Cgi::setEnvArr(HttpRequest &request) {
  envMap["GATEWAY_INTERFACE"] = "CGI/1.1";
  envMap["SERVER_PROTOCOL"] = request.version;
  envMap["REQUEST_METHOD"] = request.method;
  envMap["CONTENT_LENGTH"] = request.contentLengthString();
  envMap["CONTENT_TYPE"] = request.contentType;
  envMap["SCRIPT_NAME"] = scriptName; // path including .file
  envMap["SCRIPT_FILENAME"] = scriptFileName;
  envMap["REDIRECT_STATUS"] = status;
}

void Cgi::setArgArr() {
  size_t lastDot = scriptName.find_last_of(".");
  if (lastDot == std::string::npos) {
    status = "400";
    throw std::runtime_error("CGI: No file extension found");
  }
  std::string fileExt = scriptName.substr(lastDot);

  if (interpreter.find(fileExt) == interpreter.end()) {
    status = "400";
    throw std::runtime_error("CGI: No interpreter configured for " + fileExt);
  }
  std::vector<std::string> args = interpreter[fileExt];
  args.push_back(scriptFileName);
  argArr = vectorToArr(args);
}

char **Cgi::vectorToArr(std::vector<std::string> &v) {
  char **arr = new char *[v.size() + 1];

  for (size_t i = 0; i < v.size(); ++i) {
    arr[i] = new char[v[i].size() + 1];
    std::strcpy(arr[i], v[i].c_str());
    LOG_INFO << "arg [" << i << "] = " << arr[i];
  }
  arr[v.size()] = NULL;
  return arr;
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
