#include "Client.hpp"
#include <fstream>

void Client::readFile() {
    std::ifstream file(path.c_str());
    if (file) {
        response.status = "200";
        response.body.assign((std::istreambuf_iterator<char>(file)),
                    std::istreambuf_iterator<char>());
        file.close();
    } else {
        response.status = "404";
        response.error = true;
        writeError();
    }
}
