#include "Command.hpp"


Command::Command(Server *server) : _server(server)
{
    FUNC f[] = {&Command::commandPASS, &Command::commandNICK};

    _commands.insert(std::make_pair("PASS", f[0]));
    _commands.insert(std::make_pair("NICK", f[1]));
    // _commands.insert(make_pair("PASS", f[0]));
    // _commands.insert(make_pair("PASS", f[0]));
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



// bool Command::userIsCorrect(Client* C)  // must by update
// {
//     std::string buffer = C->getInputBuffer();
//     if(buffer.substr(0,5) != "USER ")
//         return false;
//     std::string notAllowed = " ,*?!@$:#.";
//     std::size_t pos = buffer.substr(5).find_first_of(notAllowed);
//     if (pos == std::string::npos)
//     {
//         C->setUSER(buffer.substr(5));
//         return true;
//     }
//     return false ;
// }

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
    if (!C->getPASS().empty())
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

