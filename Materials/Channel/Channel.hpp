#pragma once

#include <vector>
#include "../Client/Client.hpp"

class Client;

class Channel
{
    public:
        Channel();
        Channel(const std::string& channelName, const std::string& key);
        ~Channel();
    
    
    private:
    std::string             _channelName;
    std::string             _key;

    std::vector<Client*>    _clinets;
    Client*                 _admin;

};