#include "Server.hpp"

Server::Server(const char *port, const char *password)
{
    this->_port = std::atoi(port);
    this->_password = password;

    _command = new Command(this);
}

Server::~Server(void)
{
    delete _command;
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
//------------------------------------------------------------------------------

void    Server::initValueStruct(void)
{
    // Set up server address
    _server_addr.sin_family = AF_INET;              // This is a macro representing the address family, specifically IPv4
    _server_addr.sin_port = htons(_port);           // Convert port to network byte order
    _server_addr.sin_addr.s_addr = INADDR_ANY;      // Accept connections from any IP address
}

//-------------------------------------------      Server  Bind    ---------------

void    Server::bindListnServer(void)
{
    // Create socket
    _server_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (_server_fd == -1)
        throw Server::Excp("Error: socket creation failed!");

    // Set address reuse option:
    // (enabling the reuse of local addresses (SO_REUSEADDR) for the socket)
    // (allowing the server to quickly restart (opt = 1))
    int opt = 1;
    if (setsockopt(_server_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) == -1)
    {
        close(_server_fd);
        throw Server::Excp("Error: setting socket options failed!");
    }

    // Bind the socket to an address and port, stored in the _server_addr structure.
    if (bind(_server_fd, (struct sockaddr*)&_server_addr, sizeof(_server_addr)) == -1)
    {
        close(_server_fd);
        throw Server::Excp("Error: server binding failed!");
    }
    
    // Listen for incoming connections
    if (listen(_server_fd , SOMAXCONN) == -1)
    {
        close(_server_fd);
        throw Server::Excp("Error: server listening failed!");
    }

    // Set non-blocking mode
    if (fcntl(_server_fd, F_SETFL, O_NONBLOCK) == -1)
    {
        close(_server_fd);
        throw Server::Excp("Error: setting socket to non-blocking mode failed!");
    }
}

void    Server::ClientConnect(void)
{
    FD_ZERO(&_READ_fds);    // initializing a descriptor set _READ_fds to the null set
    FD_ZERO(&_WR_fds);      // initializing a descriptor set _WR_fds to the null set
    FD_ZERO(&_ER_fds);      // initializing a descriptor set _ER_fds to the null set

    _timeout.tv_sec = 2;    // waiting time (2 sec.)
    _timeout.tv_usec = 0;

    FD_SET(_server_fd, &_WR_fds); // adding the file descriptor _server_fd to the _WR_fds set.

    std::map<int, Client*>::iterator it = this->_Clients.begin();
    for(; it != _Clients.end(); ++it)
    {
        FD_SET(it->first, &_READ_fds);
        // FD_SET(it->first, &_WR_fds); // because clients can always write
        FD_SET(it->first, &_ER_fds);
    }
    _max_fd = _Clients.rbegin()->first + 1;

    _ready_FD = select(_max_fd, &_READ_fds, &_WR_fds, &_ER_fds, &_timeout);

    // ready for the specified events
    if (_ready_FD == -1)
    {
       this->closeFreeALL();
       throw Server::Excp("Error occurred during 'select' operation");
    }
    else if (_ready_FD == 0)
    {
        // std::cout<< "No activity within 2 seconds." << std::endl;
    }
    else
    {
        struct sockaddr_in client_addr;
        client_addr.sin_family = AF_INET;
        client_addr.sin_port = htons(_port);
        socklen_t len = sizeof(client_addr);

        // Check if the listening socket is ready
        if (FD_ISSET(_server_fd, &_READ_fds))
        {
            int client_fd = accept(_server_fd, (struct sockaddr*)&client_addr, &len);

            // verify that the file descriptor conforms to the standard list
            if (client_fd == -1)
            {
                close(_server_fd);
                throw Server::Excp("Error: Server can not accept the client");
            }
            else if (client_fd)
            {
                // if (fcntl(client_fd, F_SETFL, O_NONBLOCK) == -1)
                // {
                //     close(_server_fd);
                //     throw Server::Excp("Error: setting client fd to non-blocking mode failed!");
                // }

                std::cout << "New connection from " << inet_ntoa(client_addr.sin_addr) << ":" << ntohs(client_addr.sin_port) << std::endl;

                _Clients[client_fd] = new Client(client_fd, client_addr);

            }
        }
    }
}

//------------------------------------------------------    iterator for Map   ------------------

void    Server::ReadingforDescriptor(void)
{
    std::map<int, Client*>::iterator it = ++this->_Clients.begin(); // [0]index input _server_fd
    int sizeBuff = 0;
    char buffer[1025] = {0};

    for( ;it != this->_Clients.end(); ++it) 
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
                this->_Clients.erase(it);
                std::cout << "The client is disconnected (fd = " << it->first<< ")." << std::endl;
                --it;
            }
            else
            {
                // we need a parsing string geved                                  --------------------!!!!!!!!!
                //:Name COMMAND parameter list
                // std::cout<< "[++" << it->first <<"++]" << buffer;
                it->second->setInputBuffer(buffer, sizeBuff);
                it->second->splitBufferToList();
                it->second->setArguments();
                //--------------------------     prints  arguments
                // while (!it->second->getArguments().empty() || !it->second->getCommand().empty())
                // {
                //     std::cout<<"command=" <<it->second->getCommand()<<std::endl;
                //     std::vector<std::string>v = it->second->getArguments();
                //     int i = 0;
                //     while(!v.empty())
                //     {
                //         std::cout<<"argument =["<<i<<"]"<<"{"<<v.at(0)<<"}"<<std::endl;
                //         v.erase(v.begin());
                //         i++;
                //     }
                //     it->second->setArguments();
                // }
              
                while (!it->second->getArguments().empty() || !it->second->getCommand().empty())
                {
                    _command->commandHandler(it->second);
                    it->second->setArguments();
                }
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

//-------------------------------------------------      Manag Client   -------------------

void Server::managClient(std::map<int, Client*>::iterator it)
{
    const char *str = "hello client\n\r";
    send(it->first, str, strlen(str), 0);
}

//--------------------------------------------------      close Free ALL ------------------

void Server::closeFreeALL(void)
{
    std::map<int, Client*>::iterator it = _Clients.begin();
    for( ; it != _Clients.end(); ++it)
    {
        close(it->first);
        if (it->second)
            delete it->second;
    }
    _Clients.clear();
}


//                     -----------------------  Server start  -------------------------------


void    Server::start(void)
{
    _command->setPass(_password);
    this->initValueStruct();

    this->bindListnServer();
    this->_Clients.insert(std::pair<int, Client*>(_server_fd, NULL));
    _max_fd = _server_fd + 1;
    std::cout << "Server listening on port "<< _port << std::endl;

    for( ; ; )
    {
        this->ClientConnect();
        this->ReadingforDescriptor();
    }
    std::cout << "END :Server stopped" << std::endl;
}

//------------------------------------------------------------------------------

std::string Server::getPASS(void)
{
    return _password;
}

Client* Server::getClient(const std::string& nickname)
{
    std::map<std::string, int>::iterator it = _nickname.find(nickname);
    if (it == _nickname.end())
        return NULL;

    return _Clients.find(it->second)->second;
}


//-----------------------------------------------------------               SET   ------------------


void Server::setToMaps(Client* C)  //   all maps add Client
{
    if (!this->getClient(C->getNICK()))
        _nickname[C->getNICK()] = C->getFd();
}