#ifndef BOT_HPP
#define BOT_HPP

#include "Server.hpp"
#include "debugger.hpp"
#include <iostream>
#include <fstream>
#include <map>
#include <string>
#include <sstream>

class Bot
{
    public:
        Bot();
        ~Bot();
        void Fetch(std::string &message);
        void readFromMap(void);
    private:
        std::map<std::string, std::string> _replay;
        std::ifstream _fileData;
};

#endif