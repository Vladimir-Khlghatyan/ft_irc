#pragma once

#include <iostream>
#include <string>
#include <fstream>
#include <vector>
#include <map>
#include "../Client/Client.hpp"
#include "../Server/Server.hpp"

#include "../irc_defs.hpp"



class Server;
class Command;

typedef void (Command::*FUNC)(Client* C);

class Command
{
    public:
        Command(Server *server);
        ~Command();
        //--------------------------------------------      set     ---
        void setPass(std::string password);
        std::string getPass(void);
        bool passwordIsCorrect(Client* C);
        //---------------------------------------------   check     ---

        bool nickIsCorrect(std::string buffer);

        //---------------------------------------------   commands  ---
        void commandHandler(Client* C);
        
        void commandNICK(Client* C);
        void commandPASS(Client* C);
        void commandUSER(Client *C);
    private:
        std::string _password;
        std::map<std::string, FUNC> _commands;
        std::vector<std::string> _arg;
        Server*  _server;
};
