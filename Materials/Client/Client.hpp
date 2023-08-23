#ifndef CLIENT_HPP
# define CLIENT_HPP

#include <iostream>
#include <cstring>

class Client
{
    public:
        Client();
        Client(int fd);
        void setBuffer(char *s, int len);
        char const *getBuffer(void);
    private:
        int _fd;
        std::string _s;
};

#endif