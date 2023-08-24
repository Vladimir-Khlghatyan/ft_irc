#include "Client.hpp"

Client::Client() : _fd(0)
{
}

Client::Client(int fd, struct sockaddr_in client_addr)
{
    this->_fd = fd;
    this->_client_addr = client_addr;
}

void Client::setBuffer(char *s, int len)
{
    for(int i = 0; i < len; i++)
    {
        _s[i] = s[i];
    }
}

char const *Client::getBuffer(void)
{
    return _s.c_str();
}