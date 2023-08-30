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

    // այս մասը պետք է ստուգել, արդյոք hostname-ը ճիշտ է ստացվում
    char hostname[NI_MAXHOST];    
    gethostname(hostname, sizeof(hostname));
    _hostname = hostname;
}

std::string Client::getInputBuffer(void)
{
    return _inputBuffer;
}

std::string Client::getNick(void)
{
    return _nick;
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
        _command = _inputBuffer.substr(0, _inputBuffer.find_first_of(' '));
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

std::string	Client::getPrefix(void)
{
    std::string prefix = _nick;

    if (!_user.empty())
        prefix += "!" + _user;

    if (!_hostname.empty())
        prefix += "@" + _hostname;

    return prefix;
}

void Client::reply(const std::string& reply)
{
    std::string buffer = ":" + this->getPrefix() + " " + reply + "\r\n";

    if (send(_fd, buffer.c_str(), buffer.length(), 0) == -1)
        std::cout << "Error: can't send message to client." << std::endl;
}
