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
    std::string fullPass = "PASS " + _password;
    if (C->getInputBuffer() == fullPass)
    {        
        C->setRegistered(true);
        return true;
    }
    std::cout<<"{"<<C->getInputBuffer().substr(5, C->getSizeBuff() - 5)<<"}";
    return false ;
}