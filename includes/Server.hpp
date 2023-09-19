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

        void        initValueStruct(void);
        void        bindListenServer(void);
        void        clientConnect(void);
        void        readingforDescriptor(void);
        void        updateNickMap(Client* C, std::string& nick);
        void        removefromMaps(void);
        void        checkForCloseChannel(void);
        Channel*    createChannel(const std::string& name, const std::string& pass);
        void        addRemoveFd(Client* C);

        std::string getPASS(void);
        Client*     getClient(const std::string& nickname);
        Channel*    getChannel(std::string &name);

        void        closeAndFreeALL(void);
        void        start(void);

        class Excp : public std::exception
		{
			public:
				Excp(const char *s);
				virtual const char *what() const throw();
			private:
				char *_s;
		};

        fd_set                      _WR_fds;
        fd_set                      _READ_fds;
        fd_set                      _ER_fds;
        bool                        _ifSend;

    private:        
        struct sockaddr_in          _server_addr;
        struct timeval              _timeout;
        std::map<int, Client*>      _clients;   // fd : client
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