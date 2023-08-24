#ifndef CLIENT_HPP
# define CLIENT_HPP

#include <iostream>
#include <cstring>
#include <arpa/inet.h>

class Client
{
    public:
        Client();
        Client(int fd, struct sockaddr_in client_addr);
        void setBuffer(char *s, int len);
        char const *getBuffer(void);
    private:
        int _fd;
        std::string _s;
        std::string _port;
        struct sockaddr_in _client_addr;
};

#endif