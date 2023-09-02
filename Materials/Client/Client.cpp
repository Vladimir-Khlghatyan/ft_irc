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

void Client::setNICK(std::string nick)
{
    _nick = nick;
}

void Client::setUSER(std::string user)
{
    _user = user;
}

void Client::setPASS(std::string pass)
{
    _pass = pass;
}

void Client::setInputBuffer(const char *s, int len)
{   
    std::string a(s, len);

    this->_inputBuffer = a.substr(0, len);//for \0
    _sizeBuff = len;
}

void Client::splitBufferToList(void)
{
    std::string str = _inputBuffer;
    std::string delimiter = "\r\n";

    size_t start = 0;
    _List.clear();

    if (str.find(delimiter) == std::string::npos)
    {
        delimiter = '\n';
    }
    size_t end = str.find(delimiter);
    while (end != std::string::npos) {
        _List.push_back(str.substr(start, end - start));
        start = end + delimiter.length();
        end = str.find(delimiter, start);
    }
}


void Client::setArguments(void)
{
    bool colon = false;
    if (!_arguments.empty())
        _arguments.clear();
    _command = "";

    if (!_List.empty())
    {
        std::stringstream ss(_List.front());
        std::string word;

        if (_List.front()[0] != ' ') //before command have space
        {
            ss >> word;
            _command = word;
        }
        while (ss >> word) {
            if (!colon)
                _arguments.push_back(word);
            else
                _arguments.back() += word;
            if (word[0] == ':')
                colon = true;
        }
        _List.pop_front();          // in list arguments delete first line
    }
}

std::string Client::getCommand(void)
{
    return _command;
}
std::vector <std::string> Client::getArguments(void)
{
    return _arguments;
}
std::string Client::getPass(void)
{
    return _pass;
}

void Client::setRegistered(void)
{
    if (!_pass.empty() && _user.empty() && _nick.empty())
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
