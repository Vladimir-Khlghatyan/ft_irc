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
        void setTopic(const std::string& topic);
        void setAdmin(void);
        void setInviteOnly(bool mode);
        void setTopicMode(bool mode);

        std::string getChannelName(void);
        std::string getKey(void);
        std::string getTopic(void);

        void joinClient(Client* C);
        void kickClient(Client* C, const std::string& reason);
        bool isInChannel(Client* C);
        void replyWho(Client* C);
        bool isAdmin(Client* C);
        void sending(Client* C, const std::string& msg, const std::string& cmd);
        void nameReply(Client *C);
        void part(Client *C);
        bool emptyClients(void);
        bool isOperator(Client *C);
        bool isInviteOnly(void);
        bool topicModeIsOn(void);
        Client *getClientByNick(std::string nickname);
        void addOperator(Client *C);
        void removeOperator(Client *C);
    
    
    private:
    std::string             _channelName;
    std::string             _key;
    std::string             _topic;
    bool                    _inviteOnly;
    bool                    _topicMode;

    std::vector<Client*>    _clients;
    std::vector<Client*>    _operators;
    Client*                 _admin;

};