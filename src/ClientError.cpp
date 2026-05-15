#include "Client.hpp"
#include "Server.hpp"
#include <fstream>
#include <sys/wait.h>

void Client::writeError() {
    std::string path;
    path = server->errorsRoot + "/" + response.status + ".html";
    std::ifstream file(path.c_str());
    if (file) {
        response.body.assign((std::istreambuf_iterator<char>(file)),
                    std::istreambuf_iterator<char>());
        file.close();
    }
}

void Client::prepareErrorResponse(std::string code) {
  if (!code.empty()) response.status = code;
  writeError();
  writeHeader(".html");
  responseRaw = response.header + response.body;
}

void Client::handleTimeout() {
  prepareErrorResponse("504");
  state = SENDING;
}
