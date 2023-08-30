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

    if (a.find("\r\n"))
        this->_inputBuffer = a.substr(0, len - 1);//for \0
    else if (a.find('\n'))
        this->_inputBuffer = a.substr(0, len);
    _sizeBuff = len;
}

std::string Client::getCommand(void)
{
    return _command;
}

void Client::setCommand(void)
{
    if (!_inputBuffer.empty())
        _command = _inputBuffer.substr(0,find(' '));
}

void Client::setRegistered(void)
{
    _registered = true;
}

void Client::incrementPassTryCount(void)
{
    ++_passTryCount;
}


void Client::setRegLevel(int level)
{
    _regLevel = level;
}


bool Client::isRegistered(void)
{
    return _registered;
}
