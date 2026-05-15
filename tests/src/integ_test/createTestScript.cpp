#include <fstream>
#include <sys/stat.h>

void createTestScript(const std::string &filename, const std::string &content) {
  std::ofstream file(filename.c_str());
  file << content;
  file.close();
  chmod(filename.c_str(), 0755);
}
