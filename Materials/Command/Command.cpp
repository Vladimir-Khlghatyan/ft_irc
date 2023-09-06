#include "Command.hpp"
#include "../Debugger/debugger.hpp"

Command::Command(Server *server) : _server(server)
{
    FUNC f[] = {&Command::commandPASS, &Command::commandNICK,   //0,1
                 &Command::commandUSER, &Command::CommandPING,  //2,3
                 &Command::CommandPONG, &Command::CommandCAP,   //4,5
                 &Command::CommandJOIN, &Command::commandPRIVMSG};//6,7

    // _commands.insert(std::make_pair("", f[]));
    _commands.insert(std::make_pair("PASS", f[0]));
    _commands.insert(std::make_pair("NICK", f[1]));
    _commands.insert(std::make_pair("USER", f[2]));
    _commands.insert(std::make_pair("PING", f[3]));
    _commands.insert(std::make_pair("PONG", f[4]));
    _commands.insert(std::make_pair("CAP",  f[5]));
    _commands.insert(std::make_pair("JOIN",  f[6]));
    _commands.insert(std::make_pair("PRIVMSG", f[7]));

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
    _server->updateNickMap(C, nick);
    C->setNICK(nick);
    C->checkForRegistered();
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
     
    if (!C->isRegistered())
    {
        C->reply(ERR_NOTREGISTERED(C->getNICK()));
    }
    if (_arg.empty())
    {
        C->reply(ERR_NEEDMOREPARAMS(C->getNICK(), "PRIVMSG"));
        return ;
    }
    if (_arg.size() < 2)
    {
        C->reply(ERR_NOTEXTTOSEND(C->getNICK()));
        return ;
    }
     
    size_t i = -1;
    std::vector<std::string> targets;

    while(!_arg[++i].empty() && _arg[i][_arg[i].length() - 1] == ',')
        targets.push_back(_arg[i].substr(0, _arg[i].length() - 1));
     
    if (!_arg[i].empty())
        targets.push_back(_arg[i++]);
     
    if (_arg[i].empty())
    {
        C->reply(ERR_NOTEXTTOSEND(C->getNICK()));
        return ;
    }
     
    std::string message = _arg[i++];
    for ( ; i < _arg.size(); ++i)
        message.append(" " + _arg[i]);
    
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
            channel->sending(C, message, "PRIVMSG");
        }
        else
        {
            Client* client = _server->getClient(targets[i]);
            if (!client)
            {
                C->reply(ERR_NOSUCHNICK(C->getNICK(), targets[i]));
                return ;
            }
            client->sending(RPL_MSG(C->getPrefix(), "PRIVMSG", targets[i], message));
        }
    }
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
        C->leavingForChannels(NULL, 0);
        _server->checkForCloseCannel();
        DEBUGGER();
        return ;
    }

    DEBUGGER();
    std::string chanelName = _arg[0];
    std::string pass = _arg.size() > 1 ? _arg[1] : "";

    std::map<std::string, std::string> ch = stringToMap(chanelName, pass);
    for (std::map<std::string, std::string>::iterator it = ch.begin(); it != ch.end(); ++it) {
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