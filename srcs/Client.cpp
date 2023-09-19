#include "Client.hpp"
#include "debugger.hpp"

// ########################################################################################

Client::Client(int fd, struct sockaddr_in client_addr)
{
    this->_fd = fd;
    this->_client_addr = client_addr;

    _isRegistered = false;
    _isClosed = false;

    char hostname[NI_MAXHOST];
    getnameinfo((struct sockaddr*)&client_addr, sizeof(client_addr), hostname, NI_MAXHOST, NULL, 0, NI_NUMERICSERV);
    _hostname = hostname;
}

// ########################################################################################

Client::~Client(void)
{

}

// ########################################################################################

int Client::getFd(void) {
    return _fd;
}

// ########################################################################################

std::string Client::getInputBuffer(void) {
    return _inputBuffer;
}

// ########################################################################################

std::string Client::getPASS(void) {
    return _pass;
}

// ########################################################################################

std::string Client::getNICK(void) {
    return _nick;
}

// ########################################################################################

std::string Client::getUSER(void) {
    return _user;
}

// ########################################################################################

std::string	Client::getPrefix(void)
{
    std::string prefix = _nick;

    if (!_user.empty())
        prefix += "!" + _user;

    if (!_hostname.empty())
        prefix += "@" + _hostname;

    return prefix;
}

// ########################################################################################

std::string Client::getCommand(void) {
    return _command;
}

// ########################################################################################

std::vector <std::string> Client::getArguments(void) {
    return _arguments;
}

// ########################################################################################

std::vector<std::string> Client::getClientAtributs(void)
{
    std::vector<std::string> user;
    user.push_back(_nick);
    user.push_back(_user);
    user.push_back(_hostname);
    user.push_back(_realname);
    return user;
}

// ########################################################################################

void Client::setInputBuffer(const std::string& inputBuffer) {   
    _inputBuffer = inputBuffer;
}

// ########################################################################################

void Client::setPASS(const std::string& pass) {
    _pass = pass;
}

// ########################################################################################

void Client::setNICK(const std::string& nick)
{
    this->sending(":" + _nick + " NICK " + nick);
    _nick = nick;
}

// ########################################################################################

void Client::setUSER(const std::string& user, std::string& realname)
{
    _user = user;
    _realname = realname;
}

// ########################################################################################

void Client::setArguments(void)
{
    if (!_arguments.empty())
        _arguments.clear();
    _command = "";

    if (!_list.empty())
    {
        std::string delimiter = " ";
        std::string str = _list.front() + " ";
        size_t i;

        if (str[0] != ' ')
        {
            i = str.find(delimiter);
            _command = str.substr(0, i);
            str = str.substr(i);
        }

        i = 0;
        if (!str.empty())
        {
            i = str.find_first_not_of(" ");

            size_t end = str.find(delimiter, i);
            while (end != std::string::npos)
            {
                if (str[i] && str[i] == ':' && str[++i])
                {
                    _arguments.push_back(str.substr(i, str.length() - i - 1));
                    break ;
                }

                if (str[i])
                    _arguments.push_back(str.substr(i, end - i));

                i = str.find_first_not_of(" ", end + 1);    
                end = str.find(delimiter, i);
            }
        }

        _list.pop_front();
    }
}

// ########################################################################################

void Client::setClosed(bool state) {
    _isClosed = state;
}

// ########################################################################################

void Client::splitBufferToList(void)
{
    std::string str = _inputBuffer;
    std::string delimiter = "\r\n";

    size_t start = 0;
    _list.clear();

    if (str.find(delimiter) == std::string::npos)
        delimiter = '\n';

    size_t end = str.find(delimiter);
    while (end != std::string::npos)
    {
        _list.push_back(str.substr(start, end - start));
        start = end + delimiter.length();
        end = str.find(delimiter, start);
    }
}

// ########################################################################################

void Client::checkForRegistration(void)
{
    if (!_pass.empty() && !_user.empty() && !_nick.empty() && !_isRegistered)
    {
        _isRegistered = true;
        this->reply(RPL_WELCOME(_nick));
    }
}

// ########################################################################################

bool Client::isRegistered(void) {
    return _isRegistered;
}

// ########################################################################################

bool Client::isClosed(void) {
    return _isClosed;
}

// ########################################################################################

void Client::joinTheChannel(Channel *channel)
{
    _channels.push_back(channel);
    channel->joinClient(this);
}

// ########################################################################################

void	Client::leaveChannel(Channel* channel, std::string reason)
{
    if (!channel)
    {
        for (std::vector<Channel*>::iterator it = _channels.begin(); it != _channels.end(); ++it)
            (*it)->part(this, reason);
        _channels.clear();
    }
    else
    {
        for (std::vector<Channel*>::iterator it = _channels.begin(); it != _channels.end(); ++it)
        {
            if (*it == channel)
            {
                (*it)->part(this, reason);
                _channels.erase(it);
                break;
            }
        }
    }
}

// ########################################################################################

void Client::leaveALLChannels(const std::string& massage)
{
    DEBUGGER();
    for (std::vector<Channel*>::iterator it = _channels.begin(); it != _channels.end(); ++it)
        (*it)->kickClient(this, massage, _isClosed);
    _channels.clear();
}

// ########################################################################################

void Client::leaveALLChannelsUnexpected(void)
{
    DEBUGGER();
    for (std::vector<Channel*>::iterator it = _channels.begin(); it != _channels.end(); ++it)
        (*it)->kickClient(this, "", _isClosed);
    _channels.clear();
}

// ########################################################################################

void	Client::sending(const std::string& massage)
{
    std::string buff = massage + "\r\n";

    if (_isClosed || send(_fd, buff.c_str(), buff.length(), 0) == -1)
        std::cerr << "Error: can't send message to client." << std::endl;
}

// ########################################################################################

void Client::reply(const std::string& reply)
{
    std::string buff = ":" + this->getPrefix() + " " + reply + "\r\n";

    if (send(_fd, buff.c_str(), buff.length(), 0) == -1)
        std::cerr << "Error: can't send message to client." << std::endl;
}

// ########################################################################################
