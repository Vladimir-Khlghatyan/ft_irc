#pragma once

#include <vector>
#include <algorithm>
#include "../Client/Client.hpp"
#include "../irc_defs.hpp"

class Client;

class Channel
{
    public:
        Channel(const std::string& channelName, const std::string& key);
        ~Channel();

    public:
        void setKey(const std::string& key);
        void setAdmin(void);

        std::string getKey(void);

        void joinClient(Client* C);
        bool isInChannel(Client* C);
        bool isAdmin(Client* C);
        void sendMessage(int fd, std::string message);
        void nameReply(Client *C);
    
    
    private:
    std::string             _channelName;
    std::string             _key;

    std::vector<Client*>    _clients;
    Client*                 _admin;

};