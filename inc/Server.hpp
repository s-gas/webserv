#ifndef SERVER_HPP
# define SERVER_HPP

# include <stdint.h>
# include <string>

class Server {
    uint16_t listen;
    std::string root;
public:
    Server(std::string fileName);
};

#endif