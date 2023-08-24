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

    _is_Exit = 0;
}


//-------------------------------------------      Server  Bind    ---------------


void    Server::bindListnServer(void)
{
    int opt = 1;
    _server_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (_server_fd == -1)
        throw Server::Excp("server socket not created");

    // reused port for restart now
    setsockopt(_server_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));

    // "0.0.0.0" represents an IPv4 address 
    // when you want to bind a socket to all available
    //  network interfaces on the machine
    // inet_pton(AF_INET, "0.0.0.0", &_client_addr.sin_addr);

    if (bind(_server_fd, (struct sockaddr*)&_server_addr, sizeof(_server_addr)) == -1)
        throw Server::Excp("server can not bind");
    
    // Listen for incoming connections
    if (listen(_server_fd , SOMAXCONN) == -1)
    {
        close(_server_fd);
        throw Server::Excp("server can not listening");
    }
    fcntl(_server_fd, F_SETFL, O_NONBLOCK);
}

void    Server::ClientConnect(void)
{
    FD_ZERO(&_READ_fds);      //clear fd is set 
    FD_ZERO(&_WR_fds);      //clear fd is set
    FD_ZERO(&_ER_fds);      //clear fd is set

    _timeout.tv_sec = 2;     //time wait
    _timeout.tv_usec = 0;


    FD_SET(_server_fd, &_WR_fds);
    std::map<int, Client*>::iterator it = this->_Clients.begin();
    for( ;it != this->_Clients.end(); it++)
    {
        FD_SET(it->first, &_READ_fds);
        // FD_SET(it->first, &_WR_fds); //because clients can always write
        FD_SET(it->first, &_ER_fds);
    }
    _max_fd = (--it)->first + 1;

    _ready_FD = select(_max_fd, &_READ_fds, &_WR_fds, &_ER_fds, &_timeout);

    //ready for the specified events
    if (_ready_FD == -1)
    {
       this->closeFreeALL();
       throw Server::Excp("select all error");
    }
    else if (_ready_FD == 0)
    {
        std::cout<< "No activity within 2 seconds.\n" << std::endl;
    }
    else
    {
        _client_fd = 0;
        struct sockaddr_in client_addr;
        client_addr.sin_family = AF_INET;
        client_addr.sin_port = htons(_port);
        _len = sizeof(client_addr);

        // Check if the listening socket is ready
        if (FD_ISSET(_server_fd, &_READ_fds))
        {
            _client_fd = accept(_server_fd, (struct sockaddr*)&client_addr, &_len);

            fcntl(_client_fd, F_SETFL, O_NONBLOCK);
            // verify that the file descriptor conforms to the standard list
            if (_client_fd == -1)
            {
                close(_server_fd);
                throw Server::Excp("Error :Server can not accept Client");
            }
            else if (_client_fd)
            {
                printf("New connection from %s:%d\n", inet_ntoa(client_addr.sin_addr), ntohs(client_addr.sin_port));

                // Add the new client_fd to the set
                Client *new_Client = new Client(_client_fd, client_addr);
                this->_Clients.insert(std::pair<int, Client*>(_client_fd, new_Client));
            }
        }
    }
}



//------------------------------------------------------    iterator for Map   ------------------



void    Server::ReadingforDescriptor(void)
{
    std::map<int, Client*>::iterator it = this->_Clients.begin();
    it++;                                                           //[0]index input _server_fd
    int sizeBuff = 0;
    char buffer[4096] = {0};

    for( ;it != this->_Clients.end(); it++)
    {
        if (FD_ISSET(it->first, &_READ_fds))
        {
            sizeBuff = recv(it->first, buffer, sizeof(buffer), 0);

            if (sizeBuff == -1)
            {
                this->closeFreeALL();
                throw Server::Excp("ERROR: There was a connection issue");
            }
            else if (sizeBuff == 0)
            {
                FD_CLR(it->first, &this->_READ_fds);
                close(it->first);
                delete it->second;
                this->_Clients.erase(it->first);
                std::cout<<"There client disconnected fd=[ "<< it->first<<" ]" <<std::endl;
                it--;
            }
            else
            {
                
                it->second->setBuffer(buffer, sizeBuff);

            }
        }
        if (FD_ISSET(it->first, &_ER_fds))
        {
            std::cout<<" -------------------------&_ER_fds)) "<<std::endl;
        }
        if (FD_ISSET(it->first, &_WR_fds))
        {
            std::cout<<" +++++++++++++++++++++++++&_WR_fds))"<<std::endl;
        }
    }
}

//--------------------------------------------------      close Free ALL --------------------

void Server::closeFreeALL(void)
{
    std::map<int, Client*>::iterator it = _Clients.begin();
    for( ; it != _Clients.end(); it++)
    {
        close(it->first);
        if (it->second)
            delete it->second;
    }
    _Clients.clear();
}

//---------------------------------------------------       Server Main  -------------------


void    Server::mainServer(void)
{
    this->initValueStruct();

    this->bindListnServer();
    this->_Clients.insert(std::pair<int, Client*>(_server_fd, NULL));
    _max_fd = _server_fd + 1;
    std::cout << "Server listening on port "<< _port << std::endl;

    for( ; ;)
    {
        this->ClientConnect();
        this->ReadingforDescriptor();
    }
    std::cout<<"END :Server stopped"<<std::endl;
}
