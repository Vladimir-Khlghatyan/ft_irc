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
        std::string getInputBuffer(void);
        std::string getNICK(void);
        std::string getPASS(void);
        std::string getCommand(void);
        std::string	getPrefix(void);
        std::vector <std::string> getArguments(void);

        void setInputBuffer(const std::string &inputBuffer);
        void setNICK(std::string nick);
        void setUSER(std::string &user, std::string &realname);
        void setPASS(std::string pass);
        void setArguments(void);

        void splitBufferToList(void);
        void checkForRegistered(void);
        bool isRegistered(void);
        void leavingForChannels(Channel* channel, int mode);
        void joinToChannel(Channel *chanel);
    
        void sending(const std::string& massage);
        void reply(const std::string& reply);

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
        std::vector <std::string> _arguments;
        std::list <std::string> _List;
        std::vector<Channel*> _channels;
        
        bool        _registered;   // registration level must be 3 (password, nickname and username)
        int         _passTryCount; // max 3 
};

#endif
