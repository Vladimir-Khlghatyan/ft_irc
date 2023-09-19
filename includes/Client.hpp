#pragma once

#include <iostream>
#include <cstring>
#include <arpa/inet.h>
#include <netdb.h>
#include <unistd.h>
#include <string>
#include <vector>
#include <list>
#include <sstream>
#include "irc_defs.hpp"
#include "Channel.hpp"

class Channel;

class Client
{
    public:
        Client(int fd, struct sockaddr_in client_addr);
        ~Client();

        int         getFd(void);
        std::string getInputBuffer(void);
        std::string getPASS(void);
        std::string getNICK(void);
        std::string getUSER(void);
        std::string	getPrefix(void);
        std::string getCommand(void);
        std::vector<std::string> getArguments(void);
        std::vector<std::string> getClientAtributs(void);

        void setInputBuffer(const std::string& inputBuffer);
        void setPASS(const std::string& pass);
        void setNICK(const std::string& nick);
        void setUSER(const std::string& user, std::string& realname);        
        void setArguments(void);
        void setClosed(bool state);

        void splitBufferToList(void);
        void checkForRegistration(void);
        bool isRegistered(void);
        bool isClosed(void);
        void joinTheChannel(Channel* channel);
        void leaveChannel(Channel* channel, std::string reason);
        void leaveALLChannels(const std::string& massage);
        void leaveALLChannelsUnexpected(void);
    
        void sending(const std::string& massage);
        void reply(const std::string& reply);

        std::string _tmpBuffer;
        
    private:
        int                         _fd;
        std::string                 _port;
        struct sockaddr_in          _client_addr;
        std::string                 _pass;          // Command: PASS   Parameters: <password>
        std::string                 _nick;          // Command: NICK   Parameters: <nikname>
        std::string                 _user;
        std::string                 _realname;
        std::string                 _hostname;
        std::string                 _command;
        std::string                 _inputBuffer;
        std::vector<std::string>    _arguments;
        std::list<std::string>      _list;
        std::vector<Channel*>       _channels;        
        bool                        _isClosed;
        bool                        _isRegistered;    // registration level must be 3 (password, nickname and username)
};
