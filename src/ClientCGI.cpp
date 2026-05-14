#include "Client.hpp"
#include "Log.hpp"
#include "Server.hpp"
#include "defines.hpp"
#include "Cgi.hpp"

void Client::serveCGI() {
    try {
        response.body = Cgi(response, request, server->locations[locationIndex]).execute();
        writeCgiHeader();
    } catch (const std::exception &e) {
        LOG_ERROR << e.what();
        response.error = true;
        writeError();
        writeHeader(".html");
    }
    response.response = response.header + response.body;
    response.print();
    write(fd, response.response.c_str(), response.response.size());
}
