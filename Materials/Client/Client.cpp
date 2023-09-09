#include "Client.hpp"
#include "../Debugger/debugger.hpp"

Client::Client() : _fd(0)
{
    _registered = false;
    _passTryCount = 0;
    _ifClosed = false;
}

Client::Client(int fd, struct sockaddr_in client_addr)
{
    this->_fd = fd;
    this->_client_addr = client_addr;

    _registered = false;
    _passTryCount = 0;
    _ifClosed = false;

    // այս մասը պետք է ստուգել, արդյոք hostname-ը ճիշտ է ստացվում
    char hostname[NI_MAXHOST];    
    gethostname(hostname, sizeof(hostname));
    _hostname = hostname;
}


//-------------------------------------------------------             GET               ----------------------



int Client::getFd(void)
{
    return _fd;
}

std::string Client::getInputBuffer(void)
{
    return _inputBuffer;
}

std::string Client::getNICK(void)
{
    return _nick;
}

std::string Client::getUSER(void)
{
    return _user;
}

std::string Client::getCommand(void)
{
    return _command;
}

bool Client::ifClosed(void)
{
    return _ifClosed;
}

std::vector <std::string> Client::getArguments(void)
{
    return _arguments;
}

std::string Client::getPASS(void)
{
    return _pass;
}

std::vector<std::string> Client::getClientAtribut(void)
{
    std::vector<std::string> user;
    user.push_back(_nick);
    user.push_back(_user);
    user.push_back(_hostname);
    user.push_back(_realname);
    return user;
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


//------------------------------------              SET         -----------------------

void Client::setClosed(bool x)
{
    _ifClosed = x;
}

void Client::setNICK(std::string nick)
{
DEBUGGER();
    sending(":" + _nick + " NICK " + nick);
    _nick = nick;
}

void Client::setUSER(std::string &user, std::string &realname)
{
    _user = user;
    _realname = realname;
}

void Client::setPASS(std::string pass)
{
    _pass = pass;
}

void Client::setInputBuffer(const std::string &inputBuffer)
{   
    _inputBuffer = inputBuffer;
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
                if (str[i] && str[i] == ':' && str[++i])
                {
                    _arguments.push_back(str.substr(i, str.length() - i - 1));
                    break ;
                }
                if (str[i])
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


void Client::joinToChannel(Channel *chanel)
{
    DEBUGGER();
    _channels.push_back(chanel);
    DEBUGGER();
    chanel->joinClient(this);
    DEBUGGER();
}
//-----------------------------------------------------------           Registered            ---------------------------- 



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


void Client::checkForRegistered(void)
{
    if (!_pass.empty() && !_user.empty() && !_nick.empty() && ! _registered)
    {
        _registered = true;
        reply(RPL_WELCOME(_nick));
    }
}

bool Client::isRegistered(void)
{
    return _registered;
}



//------------------------------------------------------------              reply               -------------------------------------


void	Client::sending(const std::string& massage)
{
DEBUGGER();
    std::string buff = massage + "\r\n";
DEBUGGER();

    if (_ifClosed || send(_fd, buff.c_str(), buff.length(), 0) == -1)
        std::cout << "Error: can't send message to client." << std::endl;
DEBUGGER();
}

void Client::reply(const std::string& reply)
{
    std::string buff = ":" + this->getPrefix() + " " + reply + "\r\n";

    if (send(_fd, buff.c_str(), buff.length(), 0) == -1)
        std::cout << "Error: can't send message to client." << std::endl;
}


//-----------------------------------------------------------              living              --------------------------------------


void	Client::leavingForChannels(Channel* channel)
{
    if (!channel)
    {
        for (std::vector<Channel*>::iterator it = _channels.begin(); it != _channels.end(); ++it)
            (*it)->part(this);
        _channels.clear();
    }
    else
    {
        for (std::vector<Channel*>::iterator it = _channels.begin(); it != _channels.end(); ++it)
        {
            if (*it == channel)
            {
                (*it)->part(this);
                _channels.erase(it);
                break;
            }
        }
    }
}


void Client::leavingALLChannels(const std::string& massage)
{
    for (std::vector<Channel*>::iterator it = _channels.begin(); it != _channels.end(); ++it)
        (*it)->kickClient(this, massage);
DEBUGGER();
    _channels.clear();
DEBUGGER();
}