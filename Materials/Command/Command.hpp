#pragma once

#include <iostream>
#include <string>
#include <fstream>


class Command
{
    public:
        Command();
        ~Command();
        void setPass(std::string password);
        std::string getPass(void);
        bool PASS(int size,  std::string& s);
    private:
        std::string _password;
};
