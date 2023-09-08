#ifndef SERVER_HPP
# define SERVER_HPP

#include <iostream>
#include <iterator>
#include <arpa/inet.h> // Add this header for 'inet_addr'
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
#include <utility> // for make_pair
// #include <netdb.h> // for f
#include <netinet/in.h>
#include <stdlib.h>
#include "../Client/Client.hpp"
#include "../Command/Command.hpp"
#include "../Channel/Channel.hpp"

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

    // private:
        struct sockaddr_in          _server_addr;
        std::string                 _password;
        unsigned short              _port;
        int                         _max_fd;
        int                         _server_fd;
        int                         _ready_FD;
        struct timeval              _timeout;
        fd_set                      _WR_fds;
        fd_set                      _READ_fds;
        fd_set                      _ER_fds;
        std::map<int, Client*>      _Clients;   // fd : client
        std::map<std::string, int>  _nickname;  // nick : fd
        Command*                    _command;
        std::set<Channel*>          _channels;
        std::stack<Client*>         _removedFDs;
};

bool argsAreValid(std::string port, std::string password);

#endif