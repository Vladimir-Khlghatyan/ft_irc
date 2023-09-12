#ifndef SERVER_HPP
# define SERVER_HPP

#include <iostream>
#include <iterator>
#include <arpa/inet.h> // for 'inet_addr'
#include <sys/types.h>
#include <sys/socket.h>
#include <sstream>
#include <exception>
#include <string.h>
#include <cstring>
#include <unistd.h>
#include <fcntl.h>
#include <map>
#include <stack>
#include <iterator>
#include <set>
#include <utility>
#include <netinet/in.h>
#include <stdlib.h>
#include "Client.hpp"
#include "Command.hpp"
#include "Channel.hpp"

class Command;

class Server
{
    public:
        Server(const char *port, const char *password);
        ~Server(void);
        void initValueStruct(void);
        void bindListnServer(void);
        void ClientConnect(void);
        void ReadingforDescriptor(void);
        void start(void);
        void closeFreeALL(void);
        void managClient(std::map<int, Client*>::iterator it);
        bool correctPassword(int fdClient);
        void updateNickMap(Client* C, std::string &nick);
        void checkForCloseCannel(void);
        Channel* getChannel(std::string &name);
        Channel *createChannel(const std::string &name, const std::string &pass);
        void removefromMaps(void);
        void addRemoveFd(Client* C);
        bool isExit(void);
        void setExit(bool exit);


        std::string getPASS(void);
        Client* getClient(const std::string& nickname);

        class Excp : public std::exception
		{
			public:
				Excp(const char *s);
				virtual const char *what() const throw();
			private:
				char *_s;
		};
        struct selectDescription 
        {
            fd_set      _WR_fds;
            fd_set      _READ_fds;
            fd_set      _ER_fds;
        } Desc;

        bool                        _ifSend;
    private:
        struct sockaddr_in          _server_addr;
        struct timeval              _timeout;
        std::map<int, Client*>      _Clients;   // fd : client
        std::map<std::string, int>  _nickname;  // nick : fd
        std::set<Channel*>          _channels;
        std::stack<Client*>         _removedFDs;
        std::string                 _password;
        Command*                    _command;
        unsigned short              _port;
        int                         _max_fd;
        int                         _server_fd;
        int                         _ready_FD;
};

bool argsAreValid(std::string port, std::string password);

#endif