#include "Client.hpp"

Client::Client() : _fd(0)
{
}

Client::Client(int fd, struct sockaddr_in client_addr)
{
    this->_fd = fd;
    this->_client_addr = client_addr;
}

void Client::setBuffer(const char *s, int len)
{
    std::string a(s, len);

    this->_s = a;
}

char const *Client::getBuffer(void)
{
    return _s.c_str();
}