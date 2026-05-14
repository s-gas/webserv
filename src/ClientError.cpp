#include "Client.hpp"
#include "Server.hpp"
#include <fstream>

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
