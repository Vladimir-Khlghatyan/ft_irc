#include <iostream>
#include <string>
#include <cctype>
#include <stdlib.h>

bool argsAreValid(std::string port, std::string password)
{
    if (password.empty() || password.length() < 8)
    {
        std::cout << "Error: password must contain at least 8 symbols!" << std::endl;
        return false;
    }
    
    if (port.empty())
    {
        std::cout << "Error: empty port!" << std::endl;
        return false;
    }

    for (size_t i = 0; i < port.size(); ++i)
    {
        if (!std::isdigit(port[i]))
        {
            std::cout << "Error: port must contain only digits!" << std::endl;
            return false;
        }
    }

    int portNum = std::atoi(port.c_str());
    if (portNum <= 0 || portNum > 65535)
    {
        std::cout << "Error: port must be in range [1:65535]!" << std::endl;
        return false;
    }
    
    return true;
}