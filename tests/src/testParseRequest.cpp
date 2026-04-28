#include "tests.hpp"
#include "Http.hpp"

int testParseRequest(int N, std::string argument, HttpRequest expected) {
    std::string failure = std::string("parseRequest()  -->  ") + RED + "FAIL" + RESET;
    std::string success = std::string("parseRequest()  -->  ") + GREEN + "SUCCESS" + RESET;

    HttpRequest result = parseRequest(argument);
    if (result.method == expected.method &&
        result.version == expected.version &&
        result.contentType == expected.contentType &&
        result.body == expected.body
    ) {
        std::cout << N << ": " << success << std::endl;
        return 0;
    } else {
        std::cout << N << ": " << failure << std::endl;
        return 1;
    }
}
