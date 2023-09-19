#pragma once

#include <iostream>
#include <string>
#include <fstream>
#include <vector>
#include <map>
#include <string>
#include "Client.hpp"
#include "Server.hpp"
#include "irc_defs.hpp"
#include "Bot.hpp"

class Bot;
class Server;
class Command;

typedef void (Command::*FUNC)(Client* C);

class Command
{
    public:
        Command(Server *server);
        ~Command();

        void setPass(std::string password);
        std::string getPass(void);
        bool nickIsCorrect(std::string buffer);
        std::vector<std::string> stringSplitToVector(std::string keys);
        std::map<std::string, std::string> stringToMap(std::string& keys, std::string& values);

        void commandHandler(Client* C);        
        void commandPASS(Client* C);
        void commandNICK(Client* C);
        void commandUSER(Client* C);
        void CommandPING(Client* C);
        void CommandPONG(Client* C);
        void CommandCAP(Client* C);
        void CommandJOIN(Client* C);
        void commandPRIVMSG(Client* C);
        void commandKICK(Client* C);
        void commandINVITE(Client* C);
        void commandMODE(Client* C);
        void commandWHO(Client* C);
        void commandQUIT(Client* C);
        void commandTOPIC(Client* C);
        void commandPART(Client* C);

    private:
        std::string                 _password;
        std::map<std::string, FUNC> _commands;
        std::vector<std::string>    _arg;
        Server*                     _server;
        Bot*                        _bot;
};
