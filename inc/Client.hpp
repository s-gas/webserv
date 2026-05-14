#ifndef CLIENT_HPP
# define CLIENT_HPP

#include "Http.hpp"

class Server;

class Client {
public:
    HttpRequest request;
    HttpResponse response;
    Server *server;
    std::string path;
    int locationIndex;
    int fd;

    enum ClientState {
        READING_REQUEST,
        PROCESSING_CGI,
        SENDING_RESPONSE,
        FINISHED
    };

    Client();
    Client(Server &s, int clientFd);
    bool handleData();
    void serveFile();
    void serveCGI();
    void uploadFile();
    void deleteFile();
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

private:
    ClientState state;
    int cgiReadFd;
    pid_t cgiPid;
    time_t startTime;
    std::string requestRaw;
    std::string responseRaw;
    size_t bytesSent;
};

#endif
