#include "Client.hpp"

Client::Client() : _fd(0)
{
    _registered = false;
    _countPass = 0;
    _arguments = 0;
    _sizeBuff = 0;    
}

Client::Client(int fd, struct sockaddr_in client_addr)
{
    this->_fd = fd;
    this->_client_addr = client_addr;
}

void Client::setBuffer(const char *s, int len)
{   
    std::string a(s, len);

    // if (a.find("\r\n"))
    //     this->_s = a.substr(0, a.find("\r,\n"));
    // else if (a.find('\n'))
        this->_s = a.substr(0, a.find('\n') - 1);
    
    std::cout<<"|"<<a<<"|"<<"["<<this->_s<<"]"<<a.find('\n')<<std::endl;
    _sizeBuff = len;
}

std::string Client::getLine(void)
{
    return _s;
}

bool Client::getRegistered(void)
{
    return _registered;
}

void Client::setRegistered(bool reg)
{
    // if (_arguments == 3)
        _registered = reg;
}

void Client::setArguments(int arg)
{
    _arguments = arg;
}

int Client::getArguments(void)
{
   return _arguments;
}

int Client::countPass(void)
{
    return _countPass;
}

void Client::countPassPlus(void)
{
    ++_countPass;
}

std::string Client::getBuffer(void) const
{
    return _s;
}

int  Client::sizeBuff()
{
    return _sizeBuff;
}