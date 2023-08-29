#include "Client.hpp"

Client::Client() : _fd(0)
{
    _registered = false;
    _passTryCount = 0;
    _regLevel = 0;
    _sizeBuff = 0;
    
}

Client::Client(int fd, struct sockaddr_in client_addr)
{
    this->_fd = fd;
    this->_client_addr = client_addr;

    _registered = false;
    _passTryCount = 0;
    _regLevel = 0;
    _sizeBuff = 0;
}

std::string Client::getInputBuffer(void)
{
    return _inputBuffer;
}

int Client::getPassTryCount(void)
{
    return _passTryCount;
}

int Client::getSizeBuff(void)
{
    return _sizeBuff;
}

int Client::getRegLevel(void)
{
    return _regLevel;
}

void Client::setInputBuffer(const char *s, int len)
{   
    std::string a(s, len);

    // if (a.find("\r\n"))
    //     this->_s = a.substr(0, a.find("\r,\n"));
    // else if (a.find('\n'))
    this->_inputBuffer = a.substr(0, a.find('\n') - 1);
    
    std::cout<<"|"<<a<<"|"<<"["<<this->_inputBuffer<<"]"<<a.find('\n')<<std::endl;
    _sizeBuff = len;
}

void Client::setRegistered(bool reg)
{
    // if (_passTryCount == 3)
        _registered = reg;
}

void Client::setPassTryCount(int arg)
{
    _passTryCount = arg;
}


void Client::incrementRegLevel(void)
{
    ++_regLevel;
}


bool Client::isRegistered(void)
{
    return _registered;
}
