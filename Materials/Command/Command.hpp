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

typedef void (Command::*FUNC)(Client* C, std::vector<std::string>& arguments);

class Command
{
    public:
        Command();
        Command(Server *server);
        ~Command();
        void setPass(std::string password);
        std::string getPass(void);
        bool passwordIsCorrect(Client* C);
        void commandHandler(Client* C);
        // bool nickIsCorrect(Client* C);
        // bool userIsCorrect(Client* C);

        void commandPASS(Client* C, std::vector<std::string>& arguments);

    private:
        std::string _password;
        std::map<std::string, FUNC> _commands;
        Server*  _server;
};

