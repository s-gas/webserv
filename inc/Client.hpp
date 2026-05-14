#ifndef CLIENT_HPP
# define CLIENT_HPP

#include "Http.hpp"
#include "defines.hpp"

class Server;

class Client {
public:
    HttpRequest request;
    HttpResponse response;
    Server *server;
    std::string path;
    int locationIndex;
    int fd;

    Client();
    Client(Server &s, int clientFd);
    bool isRequestValid();
    bool isMethodAllowed();
    int isEndpoint();
    void writeHeader(std::string extension);
    void writeCgiHeader();
    void readFile();
    void writeFile();
    void generatePath();
    void writeError();
    bool isCgi();
    bool isSizeOkay();

    // State Machine
    void handleAction(int triggeredFd);
    void readRequestChunk();
    void processRequest();
    void readCgiChunk();
    void sendResponseChunk();
    void setupCgi();
    void prepareFileResponse();
    void prepareUploadResponse();
    void prepareDeleteResponse();
    void handleTimeout();
    void prepareErrorResponse(std::string code);
    bool is(ClientState s);
    int cgiReadFd;
    ClientState state;
    pid_t cgiPid;
    time_t startTime;
    std::string requestRaw;
    std::string responseRaw;
    size_t bytesSent;
};

#endif
