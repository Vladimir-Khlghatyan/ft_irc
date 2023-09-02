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

class Client
{
    public:
        Client();
        Client(int fd, struct sockaddr_in client_addr);

        std::string getInputBuffer(void);
        std::string getNick(void);
        std::string getPass(void);
        int getPassTryCount(void);
        int getSizeBuff(void);
        int getRegLevel(void);
        std::string getCommand(void);
        std::vector <std::string> getArguments(void);

        void setInputBuffer(const char *s, int len);
        void setRegistered(void);
        void setRegLevel(int level);
        void setNICK(std::string nick);
        void setUSER(std::string user);
        void setPASS(std::string pass);
        void splitBufferToVector(void);
        void setArguments(void);

        void splitBufferToList(void);
        bool isRegistered(void);
        void incrementPassTryCount(void);

        std::string	getPrefix(void);
        void reply(const std::string& reply);

    private:
        int _fd;
        std::string _inputBuffer;
        std::string _port;
        struct sockaddr_in _client_addr;
        std::string _pass; // Command: PASS   Parameters: <password>
        std::string _nick; // Command: NICK   Parameters: <nikname>
        std::string _user;
        std::string _hostname;
        std::string _command;
        std::vector <std::string> _arguments;
        std::list <std::string> _List;
        
        bool        _registered;
        int         _passTryCount; // max 3
        int         _regLevel; // registration level must be 3 (password, nickname and username)
        int         _sizeBuff;
};

#endif