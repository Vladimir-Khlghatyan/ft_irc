#include "Command.hpp"

Command::Command()
{

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

bool Command::passwordIsCorrect(Client* C)
{
    std::string fullPass = "PASS " + _password;
    if (C->getInputBuffer() == fullPass)
    {
        C->setRegLevel(1);
        return true;
    }
    return false ;
}

bool Command::nickIsCorrect(Client* C)  // // must by update
{
    std::string buffer = C->getInputBuffer();
    if(buffer.substr(0,5) != "NICK " || buffer.size() < 6)
        return false;
    std::string notAllowed = " ,*?!@$:#.";
    std::size_t pos = buffer.substr(5).find_first_of(notAllowed);
    if (pos == std::string::npos)
    {
        _nickname = buffer.substr(5);
        return true;
    }
    return false ;
}

bool Command::userIsCorrect(Client* C)  // must by update
{
    std::string buffer = C->getInputBuffer();
    if(buffer.substr(0,5) != "USER ")
        return false;
    std::string notAllowed = " ,*?!@$:#.";
    std::size_t pos = buffer.substr(5).find_first_of(notAllowed);
    if (pos == std::string::npos)
    {
        _user = buffer.substr(5,buffer.find(' '));
        return true;
    }
    return false ;
}
