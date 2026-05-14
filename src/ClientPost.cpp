#include "Client.hpp"
#include "Log.hpp"
#include "Server.hpp"
#include "defines.hpp"
#include "readRequest.hpp"

void Client::writeFile() {
    LOG_INFO << "Attempting to POST/Upload to path: " << path;

    std::ofstream file(path.c_str());
    if (!file.is_open()) {
        LOG_ERROR << "Failed to open file for writing: " << path;
        response.status = "500";
        response.error = true;
        writeError();
        return;
    }
    response.status = "201";
    response.body = "File created successfully\n";
    file << request.body;
    file.close();
}
