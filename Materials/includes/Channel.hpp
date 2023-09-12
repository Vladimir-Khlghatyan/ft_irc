#pragma once

#include <vector>
#include <algorithm>
#include "Client.hpp"
#include "Channel.hpp"
#include "irc_defs.hpp"

class Client;

class Channel
{
    public:
        Channel(const std::string& channelName, const std::string& key);
        ~Channel();

    public:
        void setKey(const std::string& key);
        void setTopic(const std::string& topic);
        void setAdmin(bool flagClosed);
        void setInviteOnly(bool mode);
        void setTopicMode(bool mode);
        void setChannelLimit(int limit);

        std::string getChannelName(void);
        std::string getKey(void);
        std::string getTopic(void);
        int getChannelLimit(void);

        void joinClient(Client* C);
        void kickClient(Client* C, const std::string& reason, bool flagClosed);
        bool isInChannel(Client* C);
        void replyWho(Client* C);
        bool isAdmin(Client* C);
        void sending(Client* C, const std::string& msg, const std::string& cmd);
        void nameReply(Client *C);
        void part(Client *C, std::string reason);
        bool emptyClients(void);
        bool isOperator(Client *C);
        bool isInviteOnly(void);
        bool topicModeIsOn(void);
        Client *getClientByNick(std::string nickname);
        void addOperator(Client *C);
        void removeOperator(Client *C);
        bool channelIsFull(void);
    
    private:
    std::string             _channelName;
    std::string             _key;
    std::string             _topic;
    bool                    _inviteOnly;
    bool                    _topicMode;
    int                     _limit;

    std::vector<Client*>    _clients;
    std::vector<Client*>    _operators;
    Client*                 _admin;

};