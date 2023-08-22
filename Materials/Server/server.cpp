#include "Server.hpp"

Server::Server(const char *port, const char *password)
{
    if (port && *port && password && *password)
    {
        this->_port = std::atoi(port);
        this->_password = password;
    }
}

//-------------------------------       Excp::Excp        ----------------------
Server::Excp::Excp(const char *s)
{
    this->_s = const_cast<char*>(s);
}

const char  *Server::Excp::what() const throw()
{
    return _s;
}


void Server::Chack(void)
{
    if (!_port)
        throw Server::Excp("three arguments must be entered, which are once digits");
    if (_port <= 0 || _port > 65535)
        throw Server::Excp("port must be entered in 0 two 65535");
    if (_password.empty() || _password.length() > 8)
        throw Server::Excp("password must be entered 8 digits");
}
void    Server::initValueStruct(void)
{
    // Forcefully attaching socket to the port for input
    // Ստիպողաբար միացնելով վարդակը  պորտին
    // Set up server address
    _server_addr.sin_family = AF_INET;// This is a macro representing the address family, specifically IPv4
    _server_addr.sin_port = htons(_port); // Port number
    _server_addr.sin_addr.s_addr = INADDR_ANY;// Accept connections from any IP address

    _client_addr.sin_family = AF_INET;
    _client_addr.sin_port = htons(_port);
    _len = sizeof(_client_addr);
    _is_Exit = 0;
}

void    Server::bindListnServer(void)
{
    _server_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (_server_fd == -1)
        throw Server::Excp("server socket not created");

    // reused port for restart now
    setsockopt(_server_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));

    // "0.0.0.0" represents an IPv4 address 
    // when you want to bind a socket to all available
    //  network interfaces on the machine
    inet_pton(AF_INET, "0.0.0.0", &_client_addr.sin_addr);

    if (bind(_server_addr,(struct sockaddr*)&_server_addr, sizeof(_server_addr)) == -1)
        throw Server::Excp("server can not bind");
    
    // Listen for incoming connections
    if (listen(listening, SOMAXCONN) == -1)
    {
        close(_server_addr);
        throw Server::Excp("server can not listening");
    }
}

void    Server::ClientConnect(void)
{
    FD_ZERO(_All_fds);      //clear fd is set 
    FD_SET(_server_fd, _All_fds);
    timeout.tv_sec = 5;     //time wait
    timeout.tv_usec = 0;

    if (is_Exit)
        break;
    _ready_fds = select(_max_fd, &_All_fds, NULL, NULL, &timeout);
    std::cout<<"//ready for the specified events  _ready_fds == "<<_ready_fds<<std::endl;//ALL fd hav events
    if (_ready_fds == -1)
    {
        close(_server_fd);
       //all  close(_client_fd);
       throw Server::Excp("select all error");
    }
    else if (_ready_fds == 0)
    {
        std::cout<< "No activity within 5 seconds.\n" << std::endl;
    }
    else
    {
        if (FD_ISSET(_server_fd, &_All_fds))
        {
            _client_fd = accept(_server_fd, (struct sockaddr*)&_client_addr, &_len);
            if (_client_fd == -1)
            {
                close(_server_fd);
                throw Server::Excp("Error :Server can not accept Client");
            }
            printf("New connection from %s:%d\n", inet_ntoa(_client_addr.sin_addr), ntohs(_client_addr.sin_port));

            // Add the new client_fd to the set
            FD_SET(_client_fd, &_All_fds);
            this->_Clients.insert(pair<int, Client>(_client_fd, Client(_client_fd)));
            if (_client_fd > _max_fd)
            {
                _max_fd = _client_fd + 1;
            }
        }
    }
}


void    Server::ReadingforDescriptor(void)
{
    std::map<int, Client>::iterator it = this->_Clients.begin();
    for( ;it != this->_Clients.end() ; it++)
    {
        //--------------------------------------------------------------------------
    }
}

void    Server::mainServer(void)
{
    int opt = 1;
    this->initValueStruct();

    this->bindListnServer();
    std::cout << "Server listening on port "<< _port << std::endl;
    _max_fd = _server_fd + 1;

    for( ; ;)
    {
        this->ClientConnect();
        // Check other file descriptors for reading
        this->ReadingforDescriptor();
    }
    std::cout<<"END :Server stopped"<<std::endl;
}
