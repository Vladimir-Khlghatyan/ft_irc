#include "Channel.hpp"

Channel::Channel(const std::string& channelName, const std::string& key) \
    : _channelName(channelName) \
    , _key(key)
{ }

Channel::~Channel()
{ }

void Channel::setKey(const std::string& key)
{
    _key = key;
}

std::string Channel::getKey(void)
{
    return _key;
}

void Channel::joinClient(Client* C)
{
    _clients.push_back(C);

    for(size_t i = 0; i < _clients.size(); ++i)
        this->sendMessage(_clients[i]->getFd(), RPL_JOIN(C->getPrefix(), _channelName));

    this->setAdmin();
    this->nameReply(C);
}

bool Channel::isInChannel(Client* C)
{
    if (find(_clients.begin(), _clients.end(), C) == _clients.end())
        return false;
    return true;
}

bool Channel::isAdmin(Client* C)
{
    if (!isInChannel(C))
        return false;
    return C == _admin;
}

void Channel::sendMessage(int fd, std::string message)
{
    message = message + "\r\n";
    if (send(fd, message.c_str(), message.size(), 0) == - 1)
        std::cout << "Error: can't send message to client." << std::endl;
}

void Channel::nameReply(Client *C)
{
    for (size_t i = 0; i < _clients.size(); ++i)
    {
        std::string prefix = (_clients[i] == _admin) ? "@" : "+";
        this->sendMessage(C->getFd(), RPL_NAMREPLY(C->getNick(), _channelName, prefix, _clients[i]->getNick()));
    }
    this->sendMessage(C->getFd(), RPL_ENDOFNAMES(C->getNick(), _channelName));
}

void Channel::setAdmin(void)
{
    if (_clients.empty() || _admin == _clients[0])
        return;

    _admin = _clients[0];

    // sending message to admin
    this->sendMessage(_admin->getFd(), RPL_MSG(_admin->getPrefix(), "", _channelName, "you are the new admin"));

    // sending message to all users expect admin (starting from index 1)
    for (size_t i = 1; i < _clients.size(); ++i)
        this->sendMessage(_clients[i]->getFd(), RPL_MSG(_admin->getPrefix(), "", _channelName, "is the new admin"));

    for(size_t i = 0; i < _clients.size(); ++i)
            this->nameReply(_clients[i]);
}