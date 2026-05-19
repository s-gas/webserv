#include "AutoIndex.hpp"
#include <sys/stat.h>
#include <time.h>

void AutoIndex::setInfo() {
  time = "-";
  size = 0;
  isDir = false;
  struct stat st;
  if (stat(path.c_str(), &st) == 0) {
    isDir = S_ISDIR(st.st_mode);
    if (isDir == true) {
      name += "/";
    }
    size = st.st_size;

    char timeBuf[20];
    struct tm* tmInfo = localtime(&st.st_mtime);
    strftime(timeBuf, 20, "%d-%b-%Y %H:%M", tmInfo);
    time = timeBuf;
  }
}
