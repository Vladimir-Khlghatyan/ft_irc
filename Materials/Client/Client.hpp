#ifndef CLIENT_HPP
# define CLIENT_HPP

#include <iostream>
#include <cstring>
#include <arpa/inet.h>
#include <netdb.h>
#include <unistd.h>
#include <string>
#include <vector>
#include <list>
#include <sstream>
#include "../irc_defs.hpp"
#include "../Channel/Channel.hpp"

class Channel;

class Client
{
    public:
        Client();
        Client(int fd, struct sockaddr_in client_addr);

        int getFd(void);
        bool ifClosed(void);
        std::string getInputBuffer(void);
        std::string getNICK(void);
        std::string getPASS(void);
        std::string getUSER(void);
        std::string getCommand(void);
        std::string	getPrefix(void);
        std::vector <std::string> getArguments(void);
        std::vector<std::string> getClientAtribut(void);

        void setInputBuffer(const std::string &inputBuffer);
        void setNICK(std::string nick);
        void setUSER(std::string &user, std::string &realname);
        void setPASS(std::string pass);
        void setArguments(void);
        void setClosed(bool x);

        void splitBufferToList(void);
        void checkForRegistered(void);
        bool isRegistered(void);
        void leavingForChannels(Channel* channel, std::string reason);
        void joinToChannel(Channel *chanel);
    
        void sending(const std::string& massage);
        void reply(const std::string& reply);
        void leavingALLChannels(const std::string& massage);

        std::string _tmpBuffer;
        
    private:
        int _fd;
        std::string _port;
        struct sockaddr_in _client_addr;
        std::string _pass; // Command: PASS   Parameters: <password>
        std::string _nick; // Command: NICK   Parameters: <nikname>
        std::string _user;
        std::string _realname;
        std::string _hostname;
        std::string _command;
        std::string _inputBuffer;
        std::vector <std::string> _arguments;
        std::list <std::string> _List;
        std::vector<Channel*> _channels;
        
        bool        _ifClosed;
        bool        _registered;   // registration level must be 3 (password, nickname and username)
        int         _passTryCount; // max 3 
};

#endif
