#include "Client.hpp"
#include "Log.hpp"
#include "Server.hpp"
#include "defines.hpp"
#include "readRequest.hpp"

void Client::deleteFile() {
    if (std::remove(path.c_str()) != 0) {
        response.status = "404";
        response.error = true;
        writeError();
    } else {
        response.status = "200";
        response.body = "File deleted successfully\n";
    }
    writeHeader(".html");
    response.response = response.header + response.body;
    response.print();
    write(fd, response.response.c_str(), response.response.size());
}
