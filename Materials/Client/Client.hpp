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
        void setBuffer(const char *s, int len);
        std::string getBuffer(void) const;
        bool getRegistered(void);
        void setRegistered(bool reg);
        int  sizeBuff();
        int  countPass(void);
        void countPassPlus(void);
        std::string getLine(void);
        void setArguments(int arg);
        int getArguments(void);
    private:
        int _fd;
        std::string _s;
        std::string _port;
        struct sockaddr_in _client_addr;
        std::string _pass; //Command: PASS   Parameters: <password>
        std::string _nick;
        std::string _user;
        int         _arguments;
        bool        _registered;
        int         _countPass;
        int         _sizeBuff;
};

#endif