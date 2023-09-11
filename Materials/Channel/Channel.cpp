#include "Channel.hpp"
#include "../Debugger/debugger.hpp"

Channel::Channel(const std::string& channelName, const std::string& key) \
    : _channelName(channelName) \
    , _key(key)
{ 
    _inviteOnly = false;
    _topicMode = true;
    _limit = 0; // 0 limit means unlimit
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

void Channel::setTopicMode(bool mode)
{
    _topicMode = mode;
}

void Channel::setChannelLimit(int limit)
{
    _limit = limit;
}

std::string Channel::getChannelName(void)
{
    return _channelName;
}

std::string Channel::getKey(void)
{
    return _key;
}

std::string Channel::getTopic(void)
{
    return _topic;
}

int Channel::getChannelLimit(void)
{
    return _limit;
}

void Channel::joinClient(Client* C)
{
    _clients.push_back(C);

    for(size_t i = 0; i < _clients.size(); ++i)
        _clients[i]->sending(RPL_JOIN(C->getPrefix(), _channelName));

    this->setAdmin(C->ifClosed());
    this->nameReply(C);
}

void Channel::kickClient(Client* C, const std::string& reason, bool flagClosed)
{
    for(size_t i = 0; i < _clients.size(); ++i)
        if (flagClosed)
            _clients[i]->sending(RPL_KICK(_admin->getPrefix(), _channelName, C->getNICK(), reason));

    std::vector<Client*>::iterator it = std::find(_clients.begin(), _clients.end(), C);
    if (it != _clients.end())
        _clients.erase(it);
    it = std::find(_operators.begin(), _operators.end(), C);
    if (it != _operators.end())
        _operators.erase(it);
    this->setAdmin(flagClosed);
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

bool Channel::topicModeIsOn(void)
{
    return _topicMode;
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
        C->sending(RPL_NOTOPIC(_channelName + static_cast<char>(1)));
    else            
        C->sending(RPL_TOPIC(_channelName + static_cast<char>(1), topic));
    
    // sending channal's users list to new user
    std::string nickList;
    for (size_t i = 0; i < _clients.size(); ++i)
    {
        std::string prefix = (_clients[i] == _admin) ? "@" : "+";
        nickList += prefix + _clients[i]->getNICK() + "  ";
    }
    C->sending(RPL_NAMREPLY(C->getNICK(), _channelName + static_cast<char>(1), nickList));
    C->sending(RPL_ENDOFNAMES(C->getNICK(), _channelName + static_cast<char>(1)));
}

void Channel::setAdmin(bool flagClosed)
{
    if (_clients.empty() || _admin == _clients[0])
        return;

    _admin = _clients[0];
    if (std::find(_operators.begin(), _operators.end(), _admin) == _operators.end())
        _operators.push_back(_admin); 

    // sending message to admin
    if (flagClosed)
        _admin->sending(RPL_MSG(_admin->getPrefix(), "", _channelName, "you are the new admin"));

    // sending message to all users expect admin (starting from index 1)
    for (size_t i = 1; i < _clients.size(); ++i)
        if (flagClosed)
            _clients[i]->sending(RPL_MSG(_admin->getPrefix(), "", _channelName, "is the new admin"));
}

void Channel::part(Client *C, std::string reason)
{
    if (std::find(_clients.begin(),_clients.end(),C) == _clients.end())
        return ;
    for(size_t i = 0; i < _clients.size(); ++i)
        if (_clients[i] != C)
            _clients[i]->sending(RPL_PART(C->getPrefix(), _channelName + reason));

    std::vector<Client*>::iterator it = std::find(_clients.begin(), _clients.end(), C);
    if (it != _clients.end())
        _clients.erase(it);
    it = std::find(_operators.begin(), _operators.end(), C);
    if (it != _operators.end())
        _operators.erase(it);
    this->setAdmin(C->ifClosed());
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

void Channel::addOperator(Client *C)
{
    if (isInChannel(C) && !isOperator(C))
        _operators.push_back(C);
}

void Channel::removeOperator(Client *C)
{
    if (isOperator(C))
    {
        std::vector<Client*>::iterator it = std::find(_operators.begin(), _operators.end(), C);
        _operators.erase(it);
    }
}

bool Channel::channelIsFull(void)
{
    if (_limit == 0)
        return false;

    return _limit <= static_cast<int>(_clients.size());
}
