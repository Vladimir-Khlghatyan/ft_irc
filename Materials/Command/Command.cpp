#include "Command.hpp"
#include "../Debugger/debugger.hpp"

Command::Command(Server *server) : _server(server)
{
    FUNC f[] = {&Command::commandPASS, &Command::commandNICK, &Command::commandUSER, &Command::CommandPING, &Command::CommandPONG};
                //\, &Command::commandPRIVMSG};

    // _commands.insert(std::make_pair("", f[]));
    _commands.insert(std::make_pair("PASS", f[0]));
    _commands.insert(std::make_pair("NICK", f[1]));
    _commands.insert(std::make_pair("USER", f[2]));
    _commands.insert(std::make_pair("PING", f[3]));
    _commands.insert(std::make_pair("PONG", f[4]));
    // _commands.insert(std::make_pair("PRIVMSG", f[5]));

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


//------------------------------------    utils command    ---------------------------



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
        return ;
    }

    std::string password;

    if (_arg[0][0] == ':')
        password = _arg[0].substr(1);
    else
        password = _arg[0];

    if (password != _server->getPASS())
    {
        C->reply(ERR_PASSWDMISMATCH(C->getNICK()));
        return ;
    }
    C->setPASS(password);
}

void Command::commandNICK(Client* C)
{
    std::cout<<"File: [" << __FILE__ << "]   line: "<< __LINE__ << "   func: [" << __func__ << "]" <<std::endl;
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

/*

void Command::commandPRIVMSG(Client *C)
{
     std::cout<<"File: [" << __FILE__ << "]   line: "<< __LINE__ << "   func: [" << __func__ << "]" <<std::endl;
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
     std::cout<<"File: [" << __FILE__ << "]   line: "<< __LINE__ << "   func: [" << __func__ << "]" <<std::endl;
    int i = 0;
    std::vector<std::string> targets;

    while(!_arg[i].empty() && _arg[i][_arg[i].length() - 1] == ','); 
    {
        targets.push_back(_arg[i].substr(0, _arg[i].length() - 1));
        ++i;
    }
     std::cout<<"File: [" << __FILE__ << "]   line: "<< __LINE__ << "   func: [" << __func__ << "]" <<std::endl;
    if (!_arg[i].empty())
    {
        targets.push_back(_arg[i]);
        i++;
    }
     std::cout<<"File: [" << __FILE__ << "]   line: "<< __LINE__ << "   func: [" << __func__ << "]" <<std::endl;
    if (_arg[i].empty())
    {
        C->reply(ERR_NOTEXTTOSEND(C->getNICK()));
        return ;
    }
     std::cout<<"File: [" << __FILE__ << "]   line: "<< __LINE__ << "   func: [" << __func__ << "]" <<std::endl;
    std::string message = _arg[i];
    for ( ; i < _arg.size(); ++i)
        message.append(" " + _arg[i]);
    i = 0
 std::cout<<"File: [" << __FILE__ << "]   line: "<< __LINE__ << "   func: [" << __func__ << "]" <<std::endl;
    
    while(targets[i])
    {
        if (targets[i][0] == '#' || targets[i][0] == '&')
        {
            Channel* channel = _server->getChannel(targets[i]);
            if (!channel)
            {
                C->reply(ERR_NOSUCHNICK(C->getNICK(), targets[i]));
                return ;
            }
            if (!channel->isAvelabel(C))
            {
                C->reply(ERR_CANNOTSENDTOCHAN(C->getNICK(), targets[i]));
                return ;
            }
            channel->sending(C, message, "PRIVMSG");
        }
        else
        {
            Client* user = _server->getClient(targets[i]);
            if (!user)
            {
                C->reply(ERR_NOSUCHNICK(C->getNICK(), targets[i]));
                return ;
            }
            user->sending(RPL_MSG(C->getPrefix(), "PRIVMSG", targets[i], message));
        }
        i++;
    }
}
*/

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

// void Command::CommandJOIN(Client *C)
// {
//     if (!C->isRegistered())
//     {
//         C->reply(ERR_NOTREGISTERED(C->getNICK()));
//         return ;
//     }
//     if (_arg.empty())
//     {
//         C->reply(ERR_NEEDMOREPARAMS(C->getNICK(), "JOIN"));
//         return ;
//     }
//     if (_arg.size() == 1 && _arg[0] == "0")
//     {
//         C->leaveChannel(0);
//         return ;
//     }
//     string channel_name = arguments[0];
//     string password = arguments.size() > 1 ? arguments[1] : "";
//     map<string, string> ch = strTransMap(channel_name, password);
//     for (map<string, string>::iterator it = ch.begin(); it != ch.end(); ++it) {
//         channel_name = it->first;
//         password = it->second;
//         if (channel_name[0] != '#' && channel_name[0] != '&')
//         {
//             client->reply(ERR_BADCHANMASK(client->getNick(), channel_name));
//             return ;
//         }
//         Channel* channel = _server->getChannel(channel_name);
//         if (!channel)
//             channel = _server->addChannel(channel_name, password);
//         else if (channel->usersSize() == 0)
//             channel->setKey(password);
//         if (channel->getKey() != "" && channel->getKey() != password)
//         {
//             client->reply(ERR_BADCHANNELKEY(client->getNick(), channel_name));
//             return ;
//         }
//         client->joinChannel(channel);
//     }
// }