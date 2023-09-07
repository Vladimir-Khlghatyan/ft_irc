#include "Command.hpp"
#include "../Debugger/debugger.hpp"

Command::Command(Server *server) : _server(server)
{
    FUNC f[] = {&Command::commandPASS, &Command::commandNICK,    //0,1
                 &Command::commandUSER, &Command::CommandPING,   //2,3
                 &Command::CommandPONG, &Command::CommandCAP,    //4,5
                 &Command::CommandJOIN, &Command::commandPRIVMSG,//6,7
                 &Command::commandKICK, &Command::commandINVITE, //8,9
                 &Command::commandMODE, &Command::commandWHO,    //10,11
                 &Command::commandQuit};                         //12

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
}

Command::~Command()
{

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
std::cout<<"command{"<<C->getCommand()<<"}"<<std::endl;
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
        return ;
    }

    std::string password = _arg[0];

    if (password != _server->getPASS())
    {
        DEBUGGER();
        C->reply(ERR_PASSWDMISMATCH(C->getNICK()));
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
        return ;
    }
    std::string nick = _arg[0];
    if (!nickIsCorrect(nick))
    {
        C->reply(ERR_ERRONEUSNICKNAME(C->getNICK(), nick));
        return ;
    }
    Client* client = _server->getClient(nick);
    if (client && client != C)
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
            channel->sending(C, message, "PRIVMSG");
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
        return ;
    }
    C->sending(RPL_PING(C->getPrefix(), _arg[0]));
}


void Command::CommandPONG(Client *C)
{
    if (_arg.empty())
    {
        C->reply(ERR_NEEDMOREPARAMS(C->getNICK(), "PONG"));
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
        C->leavingForChannels(NULL);
        _server->checkForCloseCannel();
        DEBUGGER();
        return ;
    }

    DEBUGGER();
    std::string chanelName = _arg[0];
    std::string pass = _arg.size() > 1 ? _arg[1] : "";

    std::map<std::string, std::string> ch = stringToMap(chanelName, pass);
    for (std::map<std::string, std::string>::iterator it = ch.begin(); it != ch.end(); ++it)
    {
        chanelName = it->first;
        DEBUGGER();
        pass = it->second;
        if (chanelName[0] != '#' && chanelName[0] != '&')
        {
            C->reply(ERR_BADCHANMASK(C->getNICK(), chanelName));
            DEBUGGER();
            return ;
        }
        DEBUGGER();
        Channel* channel = _server->getChannel(chanelName);
        if (!channel)
            channel = _server->createChannel(chanelName, pass);

        if (channel->isInviteOnly())
        {
            C->reply(ERR_INVITEONLYCHAN(C->getNICK(), chanelName));
            DEBUGGER();
            return ;
        }

        DEBUGGER();
        if (/*channel->getKey() != "" && */channel->getKey() != pass)
        {
            C->reply(ERR_BADCHANNELKEY(C->getNICK(), chanelName));
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
    if (_arg.empty() || _arg.size() < 2)
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
            C->reply(ERR_NOSUCHCHANNEL(C->getNICK(), channelName));
            DEBUGGER();
            return ;
        }

        Client* client = channel->getClientByNick(nickname);
        if (!client)
        {
            C->reply(ERR_USERNOTINCHANNEL(C->getNICK(), nickname, channelName));
            DEBUGGER();
            return ;
        }

        if (!channel->isInChannel(C))
        {
            C->reply(ERR_NOTONCHANNEL(C->getNICK(), channelName));
            return ;
        }

        if (!channel->isOperator(C))
        {
            C->reply(ERR_CHANOPRIVSNEEDED(C->getNICK(), channelName));
            DEBUGGER();
            return ;
        }

        if (!(channel->isAdmin(C)) && channel->isAdmin(client))
        {
            C->reply(ERR_CHANOPRIVSNEEDED(C->getNICK(), channelName));
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
        channel->kickClient(client, reason);
        DEBUGGER();
        client->leavingForChannels(channel);
        DEBUGGER();
    }
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
    if (_arg.empty() || _arg.size() < 2)
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
        std::cout << "size: " << _server->_nickname.size() << std::endl;
        DEBUGGER();
        C->reply(ERR_NOSUCHNICK(C->getNICK(), nickName));
        return ;
    }

    Channel* channel = _server->getChannel(channelName);
    if (!channel)
    {
        C->reply(ERR_NOSUCHNICK(C->getNICK(), channelName));
        DEBUGGER();
        return ;
    }

    if (!channel->isInChannel(C))
    {
        C->reply(ERR_NOTONCHANNEL(C->getNICK(), channelName));
        DEBUGGER();
        return ;
    }
    if (channel->isInChannel(client))
    {
        C->reply(ERR_USERONCHANNEL(C->getNICK(), nickName, channelName));
        DEBUGGER();
        return ;
    }

    client->sending(RPL_INVITE(C->getPrefix(), nickName, channelName));
    C->reply(RPL_INVITING(C->getNICK(), nickName, channelName));
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

    std::string nicklName = _arg[0];

    Channel* channel = _server->getChannel(nicklName);
    if (!channel)
    {
        C->reply(ERR_NOSUCHCHANNEL(C->getNICK(), nicklName));
        DEBUGGER();
        return ;
    }

    if (!channel->isInChannel(C))
    {
        C->reply(ERR_NOTONCHANNEL(C->getNICK(), nicklName));
        DEBUGGER();
        return ;
    }

    if (!channel->isOperator(C))
    {
        C->reply(ERR_CHANOPRIVSNEEDED(C->getNICK(), channel->getChannelName()));
        DEBUGGER();
        return ;
    }

    if (_arg.size() > 1)
    {
        std::string mode = _arg[1];

        if (mode == "i" || mode == "+i")
        {
            channel->setInviteOnly(true);
            C->sending(RPL_MODE(C->getPrefix(), nicklName, "+i"));
        }
        else if (mode == "-i")
        {
            channel->setInviteOnly(false);
            C->sending(RPL_MODE(C->getPrefix(), nicklName, "-i"));
        }
        else if (0)
        {
            // some code
        }
        else
        {
            C->reply(ERR_UNKNOWNMODE(C->getNICK(), mode));
            DEBUGGER();
            return;
        }
    }






    // if (_arg.size() == 1)
    // {
    //     if (!channel->getKey().empty())
    //         C->sending(RPL_MODE(C->getPrefix(), nicklName, "+k " + channel->getKey()));
    //     DEBUGGER();
    //     return ;
    // }
    // std::string mode = _arg[1];
    // if (_arg.size() > 2)
    // {
    //     DEBUGGER();
    //     std::string key = _arg[2];
    //     if (!(channel->isAdmin(C)))
    //         C->reply(ERR_CHANOPRIVSNEEDED(C->getNICK(), nicklName));
    //     else if (mode == "+k")
    //     {
    //         C->sending(RPL_MODE(C->getPrefix(), nicklName, "+k " + key));
    //         channel->setKey(key);
    //     }
    //     else if (mode == "-k" && channel->getKey() == key)
    //     {
    //         C->sending(RPL_MODE(C->getPrefix(), nicklName, "-k "));
    //         channel->setKey("");
    //     }
    // }
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
        return ;
    }

    if (_arg[0][0] == '#' || _arg[0][0] == '&')
    {
        Channel* channel = _server->getChannel(_arg[0]);
        if (!channel)
        {
            C->reply(ERR_NOSUCHNICK(C->getNICK(), _arg[0]));
            return ;
        }
        if (!channel->isInChannel(C))
        {
            C->reply(ERR_NOTONCHANNEL(C->getNICK(), _arg[0]));
            return ;
        }
        channel->replayWho(C);
        return ;
    }

    Client* client = _server->getClient(_arg[0]);
    if (!client)
    {
        C->reply(ERR_NOSUCHNICK(C->getNICK(), _arg[0]));
        return ;
    }
    std::vector<std::string> atribut = client->getClientAtribut();
    std::string messag = RPL_WHOREPLY(C->getNICK(), "*", atribut[1], atribut[2], atribut[0], "G", atribut[3]);
    C->sending(messag);
    C->sending(RPL_ENDOFWHO(C->getNICK(), _arg[0]));
}

void Command::commandQuit(Client *C)
{
    DEBUGGER();
    if (!C->isRegistered())
    {
        C->reply(ERR_NOTREGISTERED(C->getNICK()));
        DEBUGGER();
        return ;
    }
    std::string replay;
    if (!_arg.empty())
    {
        int i = 0;
        while(!_arg[i].empty())
            replay += " " + _arg[i++];
    }
    C->leavingALLChannels(replay);
    C->sending(RPL_QUIT(C->getPrefix(), replay));
    _server->deletToMaps(C);
}