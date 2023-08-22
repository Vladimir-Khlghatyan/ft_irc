#ifndef CLIENT_HPP
# define CLIENT_HPP

#include <iostream>
#include <string>

class Client
{
    public:
        Client();
        Client(int fd);
    private:
        int _fd;
};

#endif