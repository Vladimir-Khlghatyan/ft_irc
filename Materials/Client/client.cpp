#include "Client.hpp"

Client::Client() : _fd(0)
{
}

Client::Client(int fd)
{
    this->_fd = fd;
}