#include "Client.hpp"
#include <cstdio>

void Client::prepareDeleteResponse() {
  if (std::remove(path.c_str()) != 0) {
    response.status = "404";
    response.error = true;
    writeError();
  } else {
    response.status = "200";
    response.body = "<html><body><h1>File deleted successfully</h1></body></html>\n";
  }
  writeHeader(".html");
  responseRaw = response.header + response.body;
  state = SENDING_RESPONSE;
}
