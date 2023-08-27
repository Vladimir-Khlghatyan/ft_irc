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

bool Command::PASS(int size, std::string& s)
{
    if (s.substr(0,5) == "PASS " && s.substr(5, size - 5) == _password)
        return true;
    if (s.substr(0, size) == _password)
        return true;
    return false ;
}