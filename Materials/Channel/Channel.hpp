#pragma once

#include <vector>
#include <algorithm>
#include "../Client/Client.hpp"
#include "../Channel/Channel.hpp"
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
        std::string getChannelName(void);

        void joinClient(Client* C);
        void kickClient(Client* C, const std::string& reason);
        bool isInChannel(Client* C);
        bool isAdmin(Client* C);
        void sendMessage(int fd, std::string message);
        void sending(Client* C, const std::string& msg, const std::string& cmd);
        void nameReply(Client *C);
        void part(Client *C);
        bool emptyClients(void);
    
    
    private:
    std::string             _channelName;
    std::string             _key;

    std::vector<Client*>    _clients;
    Client*                 _admin;

};