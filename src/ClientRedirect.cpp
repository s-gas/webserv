#include "Client.hpp"
#include "Server.hpp"
#include "defines.hpp"
#include "Log.hpp"

bool Client::isRedirect() {
    if (locationIndex == -1) return false;
    Location &location = server->locations[locationIndex];
    return !location.redirect.empty();
}

void Client::redirect() {
    writeRedirectionHeader();
    responseRaw = response.header;
    LOG_RESPONSE
    state = SENDING;
}

void Client::writeRedirectionHeader() {
    Location &location = server->locations[locationIndex];
    response.status = "302";
    std::stringstream  ss;
    ss << response.version << " " << response.status << " " << response.statuses[response.status] << "\r\n";
    ss << "Server: " << response.server << "\r\n";
    ss << "Location: " << location.redirect << "\r\n";
    ss << "Content-Length: 0\r\n";
    ss << "\r\n";
    response.header = ss.str();
}
