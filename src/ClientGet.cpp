#include "AutoIndex.hpp"
#include "Client.hpp"
#include "Server.hpp"
#include "Log.hpp"
#include "defines.hpp"
#include <dirent.h>
#include <fstream>
#include <sstream>
#include <string>

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

void Client::prepareFileResponse() {
  Location &location = server->locations[locationIndex];
  LOG_INFO << "location.autoIndex " << location.autoIndex;
  if (location.autoIndex == true) {
    writeDirectoryList();
  } else {
    readFile();
  }
  writeHeader(".html");
  responseRaw = response.header + response.body;
  LOG_RESPONSE
  state = SENDING;
}

void Client::writeDirectoryList() {
  DIR *dir = opendir(path.c_str());
  LOG_INFO << "path " << path;
  if (!dir) {
    response.status = "403";
    response.error = true;
    writeError();
    return;
  }

  // Header / Title
  response.body +=
      "<html>\r\n<head><title>Index of " + path + "</title></head>\r\n";
  response.body += "<h1>Index of " + path + "</h1>\r\n";

  // Table-Headers
  response.body +=
      "<table>\r\n<thead>\r\n<tr><th>Name</th><th>Last Modified</th><th>Size "
      "(Bytes)</th></tr>\r\n</thead>\r\n<tbody>\r\n";

  // Looping through the Directory
  struct dirent *entry;
  while ((entry = readdir(dir)) != NULL) {
    AutoIndex file;
    file.name = entry->d_name;

    if (file.name == ".")
      continue;

    file.path = path + file.name;
    file.setInfo();

    response.body += "<tr>\r\n<td><a href=\"" + file.name + "\">" + file.name +
                     "</a></td>\r\n<td>" + file.time + "</td>\r\n";

    if (file.isDir) {
      response.body += "<td>-</td>\r\n";
    } else {
      std::stringstream fileSize;
      fileSize << file.size;
      response.body += "<td>" + fileSize.str() + "</td>\r\n";
    }
  }
  closedir(dir);
  response.body += "</tbody>\r\n</table>\r\n</body>\r\n</html>\r\n";
}
