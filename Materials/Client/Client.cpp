#include "Client.hpp"

Client::Client() : _fd(0)
{
    _registered = false;
    _passTryCount = 0;
    _sizeBuff = 0;
}

Client::Client(int fd, struct sockaddr_in client_addr)
{
    this->_fd = fd;
    this->_client_addr = client_addr;

    _registered = false;
    _passTryCount = 0;
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

std::string Client::getNICK(void)
{
    return _nick;
}

int Client::getSizeBuff(void)
{
    return _sizeBuff;
}

int Client::getRegLevel(void)
{
    return _regLevel;
}
int Client::getFd(void)
{
    return _fd;
}

void Client::setNICK(std::string nick)
{
    _nick = nick;
    sending(":" + old_nick + " NICK " + nick);
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
    if (!_arguments.empty())
        _arguments.clear();
    _command = "";

    if (!_List.empty())
    {
        std::string str(_List.front());
        std::string delimiter = " ";
        size_t end = 0;
        size_t i;
        str += ' ';

        if (str[0] != ' ')
        {
            i = str.find(delimiter);
            _command = str.substr(0, i);
            str = str.substr(i);
        }
        i = 0;
        if (!str.empty())
        {
            while(str[i] && str[i] == ' ')
                i++;
            end = str.find(delimiter, i);
            while (end != std::string::npos) {
                if (str[i] && str[i] == ':')
                {
                    _arguments.push_back(str.substr(i));
                    break ;
                }
                _arguments.push_back(str.substr(i, end - i));
                i = end + 1;
                while(str[i] && str[i] == ' ')
                    i++;
                end = str.find(delimiter, i);
            }
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

std::string Client::getPASS(void)
{
    return _pass;
}

void Client::setRegistered(void)
{
    if (!_pass.empty() && _user.empty() && _nick.empty())
        _registered = true;
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

void	Client::sending(const string& massage)
{
    string buff = massage + "\r\n";

    if (send(_fd, buff.c_str(), buff.length(), 0) == -1)
        cout << "Error: can't send message to client." << endl;
}

void Client::reply(const std::string& reply)
{
    std::string buff = ":" + this->getPrefix() + " " + reply + "\r\n";

    if (send(_fd, buff.c_str(), buff.length(), 0) == -1)
        std::cout << "Error: can't send message to client." << std::endl;
}
