#include "Channel.hpp"
#include "../Debugger/debugger.hpp"

Channel::Channel(const std::string& channelName, const std::string& key) \
    : _channelName(channelName) \
    , _key(key)
{ 
    _inviteOnly = false;
}

Channel::~Channel()
{ }

void Channel::setKey(const std::string& key)
{
    _key = key;
}

void Channel::setInviteOnly(bool mode)
{
    _inviteOnly = mode;
}

std::string Channel::getKey(void)
{
    return _key;
}

std::string Channel::getChannelName(void)
{
    return _channelName;
}

void Channel::joinClient(Client* C)
{
    DEBUGGER();
    _clients.push_back(C);

    for(size_t i = 0; i < _clients.size(); ++i)
        this->sendMessage(_clients[i]->getFd(), RPL_JOIN(C->getPrefix(), _channelName));

    DEBUGGER();
    this->setAdmin();
    DEBUGGER();
    this->nameReply(C);
    DEBUGGER();
}

void Channel::kickClient(Client* C, const std::string& reason)
{
    for(size_t i = 0; i < _clients.size(); ++i)
        this->sendMessage(_clients[i]->getFd(), RPL_KICK(_admin->getPrefix(), _channelName, C->getNICK(), reason));
        // _client[i]->sending(RPL_KICK(_admin->getPrefix(), _channelName, C->getNICK(), reason));

    std::vector<Client*>::iterator it = std::find(_clients.begin(), _clients.end(), C);
    if (it != _clients.end())
        _clients.erase(it);

    it = std::find(_operators.begin(), _operators.end(), C);
    if (it != _operators.end())
        _operators.erase(it);
    this->setAdmin();
}

bool Channel::isInChannel(Client* C)
{
    if (std::find(_clients.begin(), _clients.end(), C) == _clients.end())
        return false;
    return true;
}

Client *Channel::getClientByNick(std::string nickname)
{
    std::vector<Client*>::iterator it = _clients.begin();
    for( ; it != _clients.end(); ++it)
    {
        if ((*it)->getNICK() == nickname)
            return (*it);
    }
    return NULL;
}

bool Channel::isAdmin(Client* C)
{
    if (!isInChannel(C))
        return false;
    return C == _admin;
}

bool Channel::isOperator(Client *C)
{
    if (std::find(_operators.begin(), _operators.end(), C) == _operators.end())
        return false;
    return true;
}

bool Channel::isInviteOnly(void)
{
    return _inviteOnly;
}

void Channel::sendMessage(int fd, std::string message)
{
    message = message + "\r\n";
    if (send(fd, message.c_str(), message.size(), 0) == - 1)
        std::cout << "Error: can't send message to client." << std::endl;
}

void Channel::sending(Client* C, const std::string& msg, const std::string& cmd)
{
    for (size_t i = 0; i < _clients.size(); ++i)
        if (_clients[i] != C)
            _clients[i]->sending(RPL_MSG(C->getPrefix(), cmd, _channelName, msg));
}

void Channel::nameReply(Client *C)
{
    for (size_t i = 0; i < _clients.size(); ++i)
    {
        std::string prefix = (_clients[i] == _admin) ? "@" : "+";
        this->sendMessage(C->getFd(), RPL_NAMREPLY(C->getNICK(), _channelName, prefix, _clients[i]->getNICK()));
    }
    this->sendMessage(C->getFd(), RPL_ENDOFNAMES(C->getNICK(), _channelName));
}

void Channel::setAdmin(void)
{
    if (_clients.empty() || _admin == _clients[0])
        return;

    _admin = _clients[0];
    if (std::find(_operators.begin(), _operators.end(), _admin) == _operators.end())
        _operators.push_back(_admin);    

    // sending message to admin
    this->sendMessage(_admin->getFd(), RPL_MSG(_admin->getPrefix(), "", _channelName, "you are the new admin"));

    // sending message to all users expect admin (starting from index 1)
    for (size_t i = 1; i < _clients.size(); ++i)
        this->sendMessage(_clients[i]->getFd(), RPL_MSG(_admin->getPrefix(), "", _channelName, "is the new admin"));

    for(size_t i = 0; i < _clients.size(); ++i)
            this->nameReply(_clients[i]);
}


void Channel::part(Client *C)
{
    for(size_t i = 0; i < _clients.size(); ++i)
        this->sendMessage(_clients[i]->getFd(), RPL_PART(C->getPrefix(), _channelName));

    std::vector<Client*>::iterator it = std::find(_clients.begin(), _clients.end(), C);
    _clients.erase(it);
    this->setAdmin();
}

bool Channel::emptyClients(void)
{
    return _clients.empty();
}


