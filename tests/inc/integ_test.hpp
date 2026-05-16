#ifndef UNIT_TEST_HPP
#define UNIT_TEST_HPP

#include <string>

void createTestScript(const std::string &filename, const std::string &content);
void testCgiGet();
void testCgiPost();
void testCgiFileErrors();

#endif
