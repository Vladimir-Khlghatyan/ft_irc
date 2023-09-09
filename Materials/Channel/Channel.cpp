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

void Channel::setTopic(const std::string& topic)
{
    _topic = topic;
}

void Channel::setInviteOnly(bool mode)
{
    _inviteOnly = mode;
}

std::string Channel::getKey(void)
{
    return _key;
}

std::string Channel::getTopic(void)
{
    return _topic;
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
        _clients[i]->sending(RPL_JOIN(C->getPrefix(), _channelName));

    this->setAdmin();
    this->nameReply(C);
    DEBUGGER();
}

void Channel::kickClient(Client* C, const std::string& reason)
{
    for(size_t i = 0; i < _clients.size(); ++i)
        _clients[i]->sending(RPL_KICK(_admin->getPrefix(), _channelName, C->getNICK(), reason));

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
        if ((*it)->getNICK() == nickname)
            return (*it);

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

void Channel::sending(Client* C, const std::string& msg, const std::string& cmd)
{
    for (size_t i = 0; i < _clients.size(); ++i)
        if (_clients[i] != C)
            _clients[i]->sending(RPL_MSG(C->getPrefix(), cmd, _channelName, msg));
}

void Channel::nameReply(Client *C)
{
    // sending TOPIC to new user    
    std::string topic = this->getTopic();
    if (topic.empty())            
        C->sending(RPL_NOTOPIC(_channelName.substr(1))); // channel name without '#' (.substr(1)) to prevent KVirc wrong message
    else            
        C->sending(RPL_TOPIC(_channelName.substr(1), topic));
    
    // sending channal's users list to new user
    std::string nickList;
    for (size_t i = 0; i < _clients.size(); ++i)
    {
        std::string prefix = (_clients[i] == _admin) ? "@" : "+";
        nickList += prefix + _clients[i]->getNICK() + "  ";
    }
    C->sending(RPL_NAMREPLY(C->getNICK(), _channelName.substr(1), nickList));
    C->sending(RPL_ENDOFNAMES(C->getNICK(), _channelName.substr(1)));
}

void Channel::setAdmin(void)
{
    if (_clients.empty() || _admin == _clients[0])
        return;

    _admin = _clients[0];
    if (std::find(_operators.begin(), _operators.end(), _admin) == _operators.end())
        _operators.push_back(_admin);    

    // sending message to admin
    _admin->sending(RPL_MSG(_admin->getPrefix(), "", _channelName, "you are the new admin"));

    // sending message to all users expect admin (starting from index 1)
    for (size_t i = 1; i < _clients.size(); ++i)
        _clients[i]->sending(RPL_MSG(_admin->getPrefix(), "", _channelName, "is the new admin"));
}


void Channel::part(Client *C)
{
    for(size_t i = 0; i < _clients.size(); ++i)
        _clients[i]->sending(RPL_PART(C->getPrefix(), _channelName));

    std::vector<Client*>::iterator it = std::find(_clients.begin(), _clients.end(), C);
    _clients.erase(it);
    this->setAdmin();
}

bool Channel::emptyClients(void)
{
    return _clients.empty();
}

void Channel::replyWho(Client *C)
{
    std::string replay;
    std::vector<std::string> atribut;
    for(size_t i = 0; i < _clients.size(); ++i)
    {
        atribut = _clients[i]->getClientAtribut();
        replay = RPL_WHOREPLY(C->getNICK(), _channelName, atribut[1], atribut[2], atribut[0], "H", atribut[3]);
        C->sending(replay);
    }
    C->sending(RPL_ENDOFWHO(C->getNICK(), _channelName));
}
