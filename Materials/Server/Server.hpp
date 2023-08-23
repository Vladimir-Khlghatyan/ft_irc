#ifndef SERVER_HPP
# define SERVER_HPP

#include "../Client/Client.hpp"
#include <iostream>
#include <iterator>
#include <arpa/inet.h> // Add this header for 'inet_addr'
#include <sys/types.h>
#include <sys/socket.h>
#include <exception>
#include <string.h>
#include <cstring>
#include <unistd.h>
#include <map>
// #include <netdb.h> // for f
#include <netinet/in.h>

class Server
{
    public:
        Server(const char *port, const char *password);
        void Chack(void);
        void initValueStruct(void);
        void bindListnServer(void);
        void ClientConnect(void);
        void ReadingforDescriptor(void);
        void mainServer(void);
        class Excp : public std::exception
		{
			public:
				Excp(const char *s);
				virtual const char *what() const throw();
			private:
				char *_s;
		};
    private:
        struct sockaddr_in _server_addr, _client_addr;
        socklen_t _len;
        std::string _password;
        int _port, _max_fd;
        int _server_fd, _client_fd;
        struct timeval _timeout;
        bool _is_Exit;
        fd_set _All_fds;
        int _ready_fds;
        std::map<int, Client*> _Clients;

};



#endif