#pragma once

#include <iostream>
#include <string>
#include <fstream>
#include "../Client/Client.hpp"


class Command
{
    public:
        Command();
        ~Command();
        void setPass(std::string password);
        std::string getPass(void);
        bool PASS(Client* C);
    private:
        std::string _password;
};
