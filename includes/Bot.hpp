#pragma once

#include "Server.hpp"
#include "debugger.hpp"
#include <iostream>
#include <fstream>
#include <map>
#include <string>
#include <sstream>
#include <ctime>

class Bot
{
    public:
        Bot();
        ~Bot();
        void        fetch(std::string& message);
        void        readFromMap(void);
        std::string getCurrentTime(void);
        
    private:
        std::map<std::string, std::string>  _reply;
        std::ifstream                       _fileData;
};
