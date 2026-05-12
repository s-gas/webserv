#include "Client.hpp"
#include "Log.hpp"
#include "Server.hpp"
#include "defines.hpp"
#include "readRequest.hpp"

void Client::uploadFile() {
    writeFile();
    writeHeader(".html");
    response.response = response.header + response.body;
    response.print();
    write(fd, response.response.c_str(), response.response.size());
}

void Client::writeFile() {
    std::ofstream file(path.c_str());
    if (!file.is_open()) {
        response.status = "500";
        response.error = true;
        writeError();
        return;
    }
    response.status = "201";
    file << request.body;
    file.close();
}