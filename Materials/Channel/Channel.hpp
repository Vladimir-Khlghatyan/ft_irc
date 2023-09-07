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
        void setInviteOnly(bool mode);

        std::string getKey(void);
        std::string getChannelName(void);

        void joinClient(Client* C);
        void kickClient(Client* C, const std::string& reason);
        bool isInChannel(Client* C);
        void replayWho(Client* C);
        bool isAdmin(Client* C);
        void sendMessage(int fd, std::string message);
        void sending(Client* C, const std::string& msg, const std::string& cmd);
        void nameReply(Client *C);
        void part(Client *C);
        bool emptyClients(void);
        bool isOperator(Client *C);
        bool isInviteOnly(void);
        Client *getClientByNick(std::string nickname);
    
    
    private:
    std::string             _channelName;
    std::string             _key;
    bool                    _inviteOnly;

    std::vector<Client*>    _clients;
    std::vector<Client*>    _operators;
    Client*                 _admin;

};