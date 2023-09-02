#include "Command.hpp"

Command::Command()
{
    FUNC f[] = {&Command::commandPASS};
    //,&Command::commandNICK,&Command::commandUSER};
    _commands.insert(std::pair<std::string, FUNC>("PASS", f[0]));
}

Command::Command(Server *server) : _server(server)
{
    FUNC f[] = {&Command::commandPASS};
    //,&Command::commandNICK,&Command::commandUSER};
    // _commands.insert(std::pair<std::string, FUNC>("PASS", f[0]));
    _commands.insert(make_pair("PASS", f[0]));
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

void Command::commandHandler(Client* C)
{
    //  void (Command::*FUNC)(Client* C, std::vector<std::string>& arguments);
    if (_server->getPassword().empty())
        std::cout<<_server->getPassword()<< "   "<<C->getCommand()<<std::endl;
    std::vector<std::string> arg = C->getArguments();
    //(this->*f[DEBUG])();
    // std::map<std::string, FUNC>::iterator pp = _commands.begin();

    (this->*_commands["PASS"])(C, arg);
}

// bool Command::nickIsCorrect(Client* C)  // // must by update
// {
//     std::string buffer = C->getInputBuffer();
//     if(buffer.substr(0,5) != "NICK " || buffer.size() < 6)
//         return false;
//     std::string notAllowed = " ,*?!@$:#.";
//     std::size_t pos = buffer.substr(5).find_first_of(notAllowed);
//     if (pos == std::string::npos)
//     {
//         C->setNICK(buffer.substr(5));
//         return true;
//     }
//     return false ;
// }

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

bool Command::passwordIsCorrect(Client* C)
{
    if (C->getCommand() != "PASS")
    {
        std::cout<<"another command you can't write"<<std::endl;
        return false ;
    }
    if (!C->getArguments().empty() && C->getArguments().at(0) == _password)
    {
        C->setPASS(_password);
        return true;
    }
    else
        std::cout << "incorrect password" << std::endl;
    return false ;
}


void Command::commandPASS(Client* C, std::vector<std::string>& arguments)
{
    if (C->isRegistered())
    {
        C->reply(ERR_ALREADYREGISTERED(C->getNick()));
        return;
    }
    if (arguments.empty())
    {
        C->reply(ERR_NEEDMOREPARAMS(C->getNick(), "PASS"));
        return ;
    }
    std::string password;
    if (arguments[0][0] == ':')
        password = arguments[0].substr(1);
    else
        password = arguments[0];

    if (password != _server->getPassword())
    {
        C->reply(ERR_PASSWDMISMATCH(C->getNick()));
        return ;
    }




    // client->unlockPasswd();
    // client->registering();
    
}
