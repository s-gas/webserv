#ifndef CLIENT_HPP
# define CLIENT_HPP

#include "Http.hpp"
#include "Server.hpp"
#include <map>

class Client {
    HttpRequest request;
    HttpResponse response;
    Server &server;

    Client(Server &server);
};

#endif
