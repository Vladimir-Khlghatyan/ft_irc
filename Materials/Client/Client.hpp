#ifndef CLIENT_HPP
# define CLIENT_HPP

#include <iostream>
#include <cstring>
#include <arpa/inet.h>

class Client
{
    public:
        Client();
        Client(int fd, struct sockaddr_in client_addr);

        std::string getInputBuffer(void);
        int getPassTryCount(void);
        int getSizeBuff(void);
        int getRegLevel(void);

        void setInputBuffer(const char *s, int len);
        void setRegistered(bool reg);
        void setPassTryCount(int arg);

        void incrementRegLevel(void);
        bool isRegistered(void);

    private:
        int _fd;
        std::string _inputBuffer;
        std::string _port;
        struct sockaddr_in _client_addr;
        std::string _pass; // Command: PASS   Parameters: <password>
        std::string _nick; // Command: NICK   Parameters: <nikname>
        std::string _user;
        bool        _registered;
        int         _passTryCount; // max 3
        int         _regLevel; // registration level must be 3 (password, nickname and username)
        int         _sizeBuff;
};

#endif