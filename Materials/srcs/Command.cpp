#include "Command.hpp"
#include "debugger.hpp"
#include "Bot.hpp"

Command::Command(Server *server) : _server(server)
{
    //------         bonus           ------

    _bot = new Bot();

    //------        Mandatory   --------
    FUNC f[] = {&Command::commandPASS, &Command::commandNICK,       // 0, 1
                 &Command::commandUSER, &Command::CommandPING,      // 2, 3
                 &Command::CommandPONG, &Command::CommandCAP,       // 4, 5
                 &Command::CommandJOIN, &Command::commandPRIVMSG,   // 6, 7
                 &Command::commandKICK, &Command::commandINVITE,    // 8, 9
                 &Command::commandMODE, &Command::commandWHO,       // 10, 11
                 &Command::commandQUIT, &Command::commandTOPIC,     // 12, 13
                 &Command::commandPART};                             // 14

    // _commands.insert(std::make_pair("", f[]));
    _commands.insert(std::make_pair("PASS", f[0]));
    _commands.insert(std::make_pair("NICK", f[1]));
    _commands.insert(std::make_pair("USER", f[2]));
    _commands.insert(std::make_pair("PING", f[3]));
    _commands.insert(std::make_pair("PONG", f[4]));
    _commands.insert(std::make_pair("CAP",  f[5]));
    _commands.insert(std::make_pair("JOIN",  f[6]));
    _commands.insert(std::make_pair("PRIVMSG", f[7]));
    _commands.insert(std::make_pair("KICK", f[8]));
    _commands.insert(std::make_pair("INVITE", f[9]));
    _commands.insert(std::make_pair("MODE", f[10]));
    _commands.insert(std::make_pair("WHO", f[11]));
    _commands.insert(std::make_pair("QUIT", f[12]));
    _commands.insert(std::make_pair("TOPIC", f[13]));
    _commands.insert(std::make_pair("PART", f[14]));
}

Command::~Command()
{
    delete _bot;
}

void Command::setPass(std::string password)
{
    _password = password;
}

std::string Command::getPass(void)
{
    return _password;
}

//------------------------------------    utils     ---------------------------

bool Command::nickIsCorrect(std::string buffer)  // // must by update
{
    std::string notAllowed = " ,*?!@$:#.";
    std::size_t pos = buffer.find_first_of(notAllowed);
    if (pos != std::string::npos)
    {
        return false;
    }
    return true;
}

std::map<std::string, std::string> Command::stringToMap(std::string &keys, std::string &values)
{
    std::map<std::string, std::string> result;

    std::vector <std::string> key;
    std::vector <std::string> value;
    keys += ',';
    values += ',';
    size_t start = 0;
    size_t index = keys.find(',', start);
    while(index != std::string::npos)
    {
        key.push_back(keys.substr(start, index - start));
        start = index + 1;
        index = keys.find(',', start);
    }

    start = 0;
    index = values.find(',', start);
    while(index != std::string::npos)
    {
        value.push_back(values.substr(start, index - start));
        start = index + 1;
        index = values.find(',', start);
    }
    start = 0;
    index =  key.size();
    for( ; start < index; ++start)
    {
        if (!key[start].empty())
        {
            result[key[start]] = start >= value.size() ? "" : value[start];
        }
    }
    return result;
}

std::vector<std::string> Command::stringSplitToVector(std::string keys)
{
    std::vector<std::string> targets;

    keys += ',';
    size_t start = 0;
    size_t index = keys.find(',', start);
    while(index != std::string::npos)
    {
        targets.push_back(keys.substr(start, index - start));
        start = index + 1;
        index = keys.find(',', start);
    }
    return targets;
}

//-----------------------------------           command .... ----------------------------

void Command::commandHandler(Client* C)
{
    _arg = C->getArguments();
    std::string cmd = C->getCommand();
    std::map<std::string, FUNC>::iterator it = _commands.begin();
    for( ; it != _commands.end(); ++it)
    {
        if (it->first == cmd)
        {
            (this->*_commands[it->first])(C);
            return ;
        }
    }
    C->reply(ERR_UNKNOWNCOMMAND(C->getNICK(), C->getCommand()));
}

void Command::commandPASS(Client* C)
{
    if (!C->getPASS().empty())
    {
        C->reply(ERR_ALREADYREGISTERED(C->getNICK()));
        return;
    }
    if (_arg.empty())
    {
        C->reply(ERR_NEEDMOREPARAMS(C->getNICK(), "PASS"));
        DEBUGGER();
        return ;
    }

    std::string password = _arg[0];

    if (password != _server->getPASS())
    {
        DEBUGGER();
        C->reply( ERR_PASSWDMISMATCH (C->getNICK()) );
        return ;
    }
    C->setPASS(password);
}

void Command::commandNICK(Client* C)
{
    if (C->getPASS().empty())
    {
        C->reply(ERR_NOTREGISTERED(C->getNICK()));
        return ;
    }
    if (_arg.empty())
    {
        C->reply(ERR_NONICKNAMEGIVEN(C->getNICK()));
        DEBUGGER();
        return ;
    }
    std::string nick = _arg[0];
    if (!nickIsCorrect(nick))
    {
        C->reply(ERR_ERRONEUSNICKNAME(C->getNICK(), nick));
        return ;
    }
    Client* client = _server->getClient(nick);
    if (client)
    {
        C->reply(ERR_NICKNAMEINUSE(C->getNICK(), nick));
        return ;
    }
    DEBUGGER();
    _server->updateNickMap(C, nick);
    DEBUGGER();
    C->setNICK(nick);
    DEBUGGER();
    C->checkForRegistered();
    DEBUGGER();
}

void Command::commandUSER(Client *C)
{
    if (C->getPASS().empty())
    {
        C->reply(ERR_NOTREGISTERED(C->getNICK()));
        return ;
    }
    if (C->isRegistered())
    {
        DEBUGGER();
        C->reply(ERR_ALREADYREGISTERED(C->getNICK()));
        return ;
    }
    if (_arg.size() < 4)
    {
        C->reply(ERR_NEEDMOREPARAMS(C->getNICK(), "USER"));
        return ;
    }
    C->setUSER(_arg[0], _arg[3]);
    C->checkForRegistered();
}

void Command::commandPRIVMSG(Client *C)
{
    DEBUGGER();
    if (!C->isRegistered())
    {
        C->reply(ERR_NOTREGISTERED(C->getNICK()));
        DEBUGGER();
        return;
    }
    if (_arg.empty())
    {
        C->reply(ERR_NEEDMOREPARAMS(C->getNICK(), "PRIVMSG"));
        DEBUGGER();
        return ;
    }
    if (_arg.size() < 2)
    {
        C->reply(ERR_NOTEXTTOSEND(C->getNICK()));
        DEBUGGER();
        return ;
    }
     
    DEBUGGER();
    std::vector<std::string> targets;
    std::string keys = _arg[0];

    keys += ',';
    size_t start = 0;
    size_t index = keys.find(',', start);
    DEBUGGER();
    while(index != std::string::npos)
    {
        targets.push_back(keys.substr(start, index - start));
        start = index + 1;
        index = keys.find(',', start);
    }

    DEBUGGER();
    size_t i = 1;
    std::string message = _arg[i++];
    for ( ; i < _arg.size(); ++i)
        message.append(" " + _arg[i]);
    
    DEBUGGER();
    for (size_t i = 0; i < targets.size(); ++i)    
    {
        if (targets[i][0] == '#' || targets[i][0] == '&')
        {
            Channel* channel = _server->getChannel(targets[i]);
            if (!channel)
            {
                C->reply(ERR_NOSUCHNICK(C->getNICK(), targets[i]));
                return ;
            }
            if (!channel->isInChannel(C))
            {
                C->reply(ERR_CANNOTSENDTOCHAN(C->getNICK(), targets[i]));
                return ;
            }
            DEBUGGER();


            if (message == "BOT" || (message.find(' ') != std::string::npos
                && message.substr(0, message.find(' ')) == "BOT"))
            {
                _bot->Fetch(message);
                channel->sendingForBot(C, message, "PRIVMSG");
                DEBUGGER();
            }
            else
            {
                channel->sending(C, message, "PRIVMSG");
                DEBUGGER();
            }
        }
        else
        {
            Client* client = _server->getClient(targets[i]);
            if (!client)
            {
                C->reply(ERR_NOSUCHNICK(C->getNICK(), targets[i]));
                DEBUGGER();
                return ;
            }
            DEBUGGER();

            client->sending(RPL_MSG(C->getPrefix(), "PRIVMSG", targets[i], message));
        }
    }
    DEBUGGER();
}


void Command::CommandCAP(Client *C)
{
    (void)*C;
}

void Command::CommandPING(Client *C)
{
    if (_arg.empty())
    {
        C->reply(ERR_NEEDMOREPARAMS(C->getNICK(), "PING"));
        DEBUGGER();
        return ;
    }
    C->sending(RPL_PING(C->getPrefix(), _arg[0]));
}


void Command::CommandPONG(Client *C)
{
    if (_arg.empty())
    {
        C->reply(ERR_NEEDMOREPARAMS(C->getNICK(), "PONG"));
        DEBUGGER();
        return ;
    }
    C->sending(RPL_PING(C->getPrefix(), _arg[0]));
}

void Command::CommandJOIN(Client *C)
{
    if (!C->isRegistered())
    {
        C->reply(ERR_NOTREGISTERED(C->getNICK()));
        DEBUGGER();
        return ;
    }

    if (_arg.empty())
    {
        C->reply(ERR_NEEDMOREPARAMS(C->getNICK(), "JOIN"));
        DEBUGGER();
        return ;
    }

    if (_arg[0] == "0")
    {
        C->leavingForChannels(NULL, "");
        _server->checkForCloseCannel();
        DEBUGGER();
        return ;
    }

    DEBUGGER();
    std::string channelName = _arg[0];
    std::string pass = _arg.size() > 1 ? _arg[1] : "";

    std::map<std::string, std::string> ch = stringToMap(channelName, pass);
    for (std::map<std::string, std::string>::iterator it = ch.begin(); it != ch.end(); ++it)
    {
        channelName = it->first;
        DEBUGGER();
        pass = it->second;
        if (channelName[0] != '#' && channelName[0] != '&')
        {
            C->reply(ERR_BADCHANMASK(C->getNICK(), channelName + static_cast<char>(1)));
            DEBUGGER();
            return ;
        }
        Channel* channel = _server->getChannel(channelName);
        if (!channel)
        {
            channel = _server->createChannel(channelName, pass);
            DEBUGGER();
        }

        if (channel->isInChannel(C))
        {
            C->reply(ERR_USERONCHANNEL(C->getNICK(), C->getNICK(), channelName + static_cast<char>(1)));
            DEBUGGER();
            return ;
        }

        if (channel->isInviteOnly())
        {
            C->reply(ERR_INVITEONLYCHAN(C->getNICK(), channelName + static_cast<char>(1)));
            DEBUGGER();
            return ;
        }

        if (channel->channelIsFull())
        {
            C->reply(ERR_CHANNELISFULL(C->getNICK(), channelName + static_cast<char>(1)));
            DEBUGGER();
            return ;
        }

        if (channel->getKey() != pass)
        {
            C->reply(ERR_BADCHANNELKEY(C->getNICK(), channelName, "Cannot join channel (+k)"));
            DEBUGGER();
            return ;
        }
        C->joinToChannel(channel);

        
        DEBUGGER();
    }
}

void Command::commandKICK(Client *C)
{
    DEBUGGER();
    if (!C->isRegistered())
    {
        C->reply(ERR_NOTREGISTERED(C->getNICK()));
        DEBUGGER();
        return ;
    }
    if (_arg.size() < 2)
    {
        C->reply(ERR_NEEDMOREPARAMS(C->getNICK(), "KICK"));
        DEBUGGER();
        return ;
    }
    DEBUGGER();
    std::map<std::string, std::string> target = stringToMap(_arg[0], _arg[1]);
    DEBUGGER();
    std::map<std::string, std::string>::iterator it = target.begin();
    DEBUGGER();
    for( ; it != target.end(); ++it)
    {
        std::string channelName = it->first;
        std::string nickname = it->second;

        Channel* channel = _server->getChannel(channelName);
        if (!channel)
        {
            C->reply(ERR_NOSUCHCHANNEL(C->getNICK(), channelName + static_cast<char>(1)));
            DEBUGGER();
            return ;
        }

        Client* client = channel->getClientByNick(nickname);
        if (!client)
        {
            C->reply(ERR_USERNOTINCHANNEL(C->getNICK(), nickname, channelName + static_cast<char>(1)));
            DEBUGGER();
            return ;
        }

        if (!channel->isInChannel(C))
        {
            C->reply(ERR_NOTONCHANNEL(C->getNICK(), channelName + static_cast<char>(1)));
            return ;
        }

        if (!channel->isOperator(C))
        {
            C->reply(ERR_CHANOPRIVSNEEDED(C->getNICK(), channelName + static_cast<char>(1)));
            DEBUGGER();
            return ;
        }

        if (!(channel->isAdmin(C)) && channel->isAdmin(client))
        {
            C->reply(ERR_CHANOPRIVSNEEDED(C->getNICK(), channelName + static_cast<char>(1)));
            DEBUGGER();
            return ;
        }

        std::string reason;
        
        DEBUGGER();
        if (_arg.size() > 2)
            for (size_t i = 2; i < _arg.size(); ++i)
                reason.append(" " + _arg[i]);
        else
            reason = "No reason specified.";

        DEBUGGER();
        channel->kickClient(client, reason, client->ifClosed());
        DEBUGGER();
        client->leavingForChannels(channel, reason);
        DEBUGGER();
    }
    _server->checkForCloseCannel();
}

void Command::commandINVITE(Client *C)
{
    DEBUGGER();
    if (!C->isRegistered())
    {
        C->reply(ERR_NOTREGISTERED(C->getNICK()));
        DEBUGGER();
        return ;
    }

    if (_arg.size() < 2)
    {
        C->reply(ERR_NEEDMOREPARAMS(C->getNICK(), "WHO"));
        DEBUGGER();
        return ;
    }

    std::string nickName = _arg[0];
    std::string channelName = _arg[1];

    Client* client = _server->getClient(nickName);
    if (!client)
    {
        DEBUGGER();
        C->reply(ERR_NOSUCHNICK(C->getNICK(), nickName));
        return ;
    }

    Channel* channel = _server->getChannel(channelName);
    if (!channel)
    {
        C->reply(ERR_NOSUCHCHANNEL(C->getNICK(), channelName + static_cast<char>(1)));
        DEBUGGER();
        return ;
    }

    if (!channel->isInChannel(C))
    {
        C->reply(ERR_NOTONCHANNEL(C->getNICK(), channelName + static_cast<char>(1)));
        DEBUGGER();
        return ;
    }

    if (!channel->isOperator(C))
    {
        C->reply(ERR_CHANOPRIVSNEEDED(C->getNICK(), channelName + static_cast<char>(1)));
        DEBUGGER();
        return ;
    }

    if (channel->isInChannel(client))
    {
        C->reply(ERR_USERONCHANNEL(C->getNICK(), nickName, channelName + static_cast<char>(1)));
        DEBUGGER();
        return ;
    }

    if (channel->channelIsFull())
    {
        C->reply(ERR_CHANNELISFULL(C->getNICK(), channelName + static_cast<char>(1)));
        DEBUGGER();
        return ;
    }

    client->sending(RPL_INVITE(C->getPrefix(), nickName, channelName));
    C->reply(RPL_INVITING(C->getNICK(), nickName, channelName + static_cast<char>(1)));
    channel->joinClient(client);
    DEBUGGER();
}


void Command::commandMODE(Client *C)
{
    DEBUGGER();
    if (!C->isRegistered())
    {
        C->reply(ERR_NOTREGISTERED(C->getNICK()));
        DEBUGGER();
        return ;
    }

    if (_arg.empty())
    {
        C->reply(ERR_NEEDMOREPARAMS(C->getNICK(), "MODE"));
        DEBUGGER();
        return ;
    }

    std::string channelName = _arg[0];

    Channel* channel = _server->getChannel(channelName);
    if (!channel)
    {
        C->reply(ERR_NOSUCHCHANNEL(C->getNICK(), channelName + static_cast<char>(1)));
        DEBUGGER();
        return ;
    }

    if (!channel->isInChannel(C))
    {
        C->reply(ERR_NOTONCHANNEL(C->getNICK(), channelName + static_cast<char>(1)));
        DEBUGGER();
        return ;
    }

    if (!channel->isOperator(C))
    {
        C->reply(ERR_CHANOPRIVSNEEDED(C->getNICK(), channelName + static_cast<char>(1)));
        DEBUGGER();
        return ;
    }

    if (_arg.size() > 1)
    {
        std::string mode = _arg[1];

        // MODE +/- i
        if (mode == "i" || mode == "+i" || mode == "-i")
        {
            DEBUGGER();
            bool condition = (mode == "-i") ? false : true;
            mode = (mode == "i") ? "+i" : mode;

            channel->setInviteOnly(condition);
            C->sending(RPL_MODE(C->getPrefix(), channelName, mode));
            C->reply(RPL_CHANNELMODEIS(channelName, channelName + static_cast<char>(1), mode));
        }
        // MODE +/- t
        else if (mode == "t" || mode == "+t" || mode == "-t")
        {
            DEBUGGER();
            bool condition = (mode == "-t") ? false : true;
            mode = (mode == "t") ? "+t" : mode;

            channel->setTopicMode(condition);
            C->sending(RPL_MODE(C->getPrefix(), channelName, mode));
            C->reply(RPL_CHANNELMODEIS(channelName, channelName + static_cast<char>(1), mode));
        }
        // MODE +/- k
        else if (mode == "k" || mode == "+k" || mode == "-k")
        {
            std::string key;
            if (_arg.size() > 2)
                key = _arg[2];

            if (mode != "-k")
            {
                if (channel->getKey() != "")
                {
                    C->reply(ERR_KEYSET(C->getNICK(), mode));
                    DEBUGGER();
                    return;
                }
                
                DEBUGGER();
                channel->setKey(key);
                C->sending(RPL_MODE(C->getPrefix(), channelName, "+k " + key));
            }
            else
            {
                if (channel->getKey() == key)
                {
                    DEBUGGER();
                    channel->setKey("");
                    C->sending(RPL_MODE(C->getPrefix(), channelName, "-k "));
                }
                else
                {
                    C->reply(ERR_BADCHANNELKEY(C->getNICK(), mode, ":invalid key"));
                    DEBUGGER();
                    return ;
                }
            }
            C->reply(RPL_CHANNELMODEIS(channelName, channelName + static_cast<char>(1), mode));
        }
        // MODE +/- o
        else if (mode == "o" || mode == "+o" || mode == "-o")
        {
            if (_arg.size() < 3)
            {
                C->reply(ERR_NEEDMOREPARAMS(C->getNICK(), "MODE"));
                DEBUGGER();
                return ;
            }

            std::string nickname = _arg[2];
            Client* client = channel->getClientByNick(nickname);
            if (!client)
            {
                C->reply(ERR_USERNOTINCHANNEL(C->getNICK(), nickname, channelName + static_cast<char>(1)));
                DEBUGGER();
                return ;
            }

            if (mode != "-o")
            {
                DEBUGGER();
                client->sending(RPL_MSG(C->getPrefix(), "MODE", channelName, ":you are now a channel operator"));
                channel->addOperator(client);
            }
            else
            {
                if (!channel->isAdmin(client) && channel->isOperator(client))
                {
                    DEBUGGER();
                    client->sending(RPL_MSG(C->getPrefix(), "MODE", channelName, ":you are no longer a channel operator"));
                    channel->removeOperator(client);
                }
            }
            C->reply(RPL_CHANNELMODEIS(channelName, channelName + static_cast<char>(1), mode));
        }
        // MODE +/- l
        else if (mode == "l" || mode == "+l" || mode == "-l")
        {
            if (mode != "-l")
            {
                if (_arg.size() < 3)
                {
                    C->reply(ERR_NEEDMOREPARAMS(C->getNICK(), "MODE"));
                    DEBUGGER();
                    return ;
                }

                int new_limit = std::atoi(_arg[2].c_str());
                if (new_limit < 1)
                {
                    C->reply(ERR_UNKNOWNMODE(C->getNICK(), mode, " :limit must be greater than 0"));
                    DEBUGGER();
                    return;
                }
                channel->setChannelLimit(new_limit);
            }
            else
            {
                channel->setChannelLimit(0); // unlimit
            }
            
            C->reply(RPL_CHANNELMODEIS(channelName, channelName + static_cast<char>(1), mode));
        }
        else if (mode == "b")
        {
            // do nothink to prevent KVirc error message
            DEBUGGER();
        }
        else
        {
            C->reply(ERR_UNKNOWNMODE(C->getNICK(), mode, " :is unknown mode char to me"));
            DEBUGGER();
            return;
        }
    }
}


void Command::commandWHO(Client *C)
{
    DEBUGGER();

    if (!C->isRegistered())
    {
        C->reply(ERR_NOTREGISTERED(C->getNICK()));
        DEBUGGER();
        return ;
    }

    if (_arg.empty())
    {
        C->reply(ERR_NEEDMOREPARAMS(C->getNICK(), "WHO"));
        DEBUGGER();
        return ;
    }

    if (_arg[0][0] == '#' || _arg[0][0] == '&')
    {
        Channel* channel = _server->getChannel(_arg[0]);
        if (!channel)
        {
            C->reply(ERR_NOSUCHCHANNEL(C->getNICK(), _arg[0]));
            return ;
        }
        if (!channel->isInChannel(C))
        {
            C->reply(ERR_NOTONCHANNEL(C->getNICK(), _arg[0]));
            return ;
        }
        int mode = (_arg.size() > 1 && _arg[1] == "o") ? 1 : 0;
        channel->replyWho(C, mode);
        return ;
    }

    Client* client = _server->getClient(_arg[0]);
    if (!client)
    {
        C->reply(ERR_NOSUCHNICK(C->getNICK(), _arg[0]));
        return ;
    }
    std::vector<std::string> atribut = client->getClientAtribut();

    C->sending(RPL_WHOREPLY(C->getNICK(), "*", atribut[1], atribut[2], atribut[0], "H", atribut[3]));
    C->sending(RPL_ENDOFWHO(C->getNICK(), _arg[0]));
}

void Command::commandQUIT(Client *C)
{
    DEBUGGER();

    std::string reply;
    if (!_arg.empty())
    {
        DEBUGGER();
        reply = _arg[0];
        for(size_t i = 1; i < _arg.size(); ++i)
            reply += " " + _arg[i];
    }
    DEBUGGER();
    C->leavingALLChannels(reply);
    DEBUGGER();

    DEBUGGER();
    close(C->getFd());
    DEBUGGER();
    FD_CLR(C->getFd(), &_server->Desc._READ_fds);
    _server->addRemoveFd(C);
    DEBUGGER();
}

void Command::commandTOPIC(Client *C)
{
    DEBUGGER();
    if (!C->isRegistered())
    {
        C->reply(ERR_NOTREGISTERED(C->getNICK()));
        DEBUGGER();
        return ;
    }

    if (_arg.empty())
    {
        C->reply(ERR_NEEDMOREPARAMS(C->getNICK(), "TOPIC"));
        DEBUGGER();
        return ;
    }

    std::string channelName = _arg[0];
    Channel* channel = _server->getChannel(channelName);
    if (!channel)
    {
        C->reply(ERR_NOSUCHCHANNEL(C->getNICK(), channelName + static_cast<char>(1)));
        DEBUGGER();
        return ;
    }
    if (!channel->isInChannel(C))
    {
        C->reply(ERR_NOTONCHANNEL(C->getNICK(), channelName + static_cast<char>(1)));
        DEBUGGER();
        return ;
    }

    if (!channel->isOperator(C))
    {
        C->reply(ERR_CHANOPRIVSNEEDED(C->getNICK(), channelName + static_cast<char>(1)));
        DEBUGGER();
        return ;
    }

    if (channel->topicModeIsOn() == false)
    {
        C->sending(ERR_NOCHANMODES(channelName));
        DEBUGGER();
        return;
    }

    if (_arg.size() == 1)
    {
        DEBUGGER();

        std::string topic = channel->getTopic();
        if (topic.empty())
        {
            DEBUGGER();
            C->sending(RPL_NOTOPIC(channelName) + static_cast<char>(1));
        }        
        else 
        {
            DEBUGGER();
            C->sending(RPL_TOPIC(channelName + static_cast<char>(1), topic));
        }          
    }
    else
    {
        std::string topic = _arg[1];
        channel->setTopic(topic);
    }
}

void Command::commandPART(Client *C)
{
    DEBUGGER();
    if (!C->isRegistered())
    {
        C->reply(ERR_NOTREGISTERED(C->getNICK()));
        DEBUGGER();
        return ;
    }
     DEBUGGER();
    if (_arg.empty())
    {
        C->reply(ERR_NEEDMOREPARAMS(C->getNICK(), "PART"));
        return ;
    }
     DEBUGGER();
    std::vector<std::string> removeChannels = stringSplitToVector(_arg[0]);
    std::vector<std::string>::iterator it = removeChannels.begin();
    std::string reason;
    DEBUGGER();
    if (_arg.size())
    {
        DEBUGGER();
        reason = _arg[0];
        for(size_t i = 1; i < _arg.size(); ++i)
            reason += " " + _arg[i];
    }
    DEBUGGER();
    std::string channelName;
    Channel* channel;
    for( ; it != removeChannels.end(); ++it)
    {
        channelName = *it;

        channel = _server->getChannel(channelName);
        if (!channel)
        {
            C->reply(ERR_NOSUCHCHANNEL(C->getNICK(), channelName));
             DEBUGGER();
            return ;
        }
        if (!channel->isInChannel(C))
        {
            C->reply(ERR_NOTONCHANNEL(C->getNICK(), channelName));
             DEBUGGER();
            return ;
        }
        C->leavingForChannels(channel, reason);
    }
     DEBUGGER();
    _server->checkForCloseCannel();
     DEBUGGER();
}
