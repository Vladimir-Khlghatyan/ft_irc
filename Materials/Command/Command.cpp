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

bool Command::PASS(Client* C)
{
    if (C->getLine().substr(0,5) == "PASS " && C->getLine().substr(5, C->sizeBuff() - 5) == _password)
    {
        
        C->setArguments(true);
        return true;
    }
    std::cout<<"{"<<C->getLine().substr(5, C->sizeBuff() - 5)<<"}";
    return false ;
}