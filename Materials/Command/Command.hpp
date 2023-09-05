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
        
        void commandNICK(Client* C);//0
        void commandPASS(Client* C);//1
        void commandUSER(Client *C);//2
        void CommandPING(Client *C);//3
        void CommandPONG(Client *C);//4
        void CommandCAP(Client *C); //5
        // void Command::CommandJOIN(Client *C);
        // void commandPRIVMSG(Client *C);
    private:
        std::string _password;
        std::map<std::string, FUNC> _commands;
        std::vector<std::string> _arg;
        Server*  _server;
};
