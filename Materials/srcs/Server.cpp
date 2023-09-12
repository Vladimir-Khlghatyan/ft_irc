#include "Server.hpp"
#include "debugger.hpp"

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
    FD_ZERO(&Desc._READ_fds);    // initializing a descriptor set Desc._READ_fds to the null set
    FD_ZERO(&Desc._WR_fds);      // initializing a descriptor set Desc._WR_fds to the null set
    FD_ZERO(&Desc._ER_fds);      // initializing a descriptor set Desc._ER_fds to the null set

    _timeout.tv_sec = 2;    // waiting time (2 sec.)
    _timeout.tv_usec = 0;

    FD_SET(_server_fd, &Desc._WR_fds); // adding the file descriptor _server_fd to the Desc._WR_fds set.

    std::map<int, Client*>::iterator it = this->_Clients.begin();
    for(; it != _Clients.end(); ++it)
    {
        FD_SET(it->first, &Desc._READ_fds);
        // FD_SET(it->first, &Desc._WR_fds); // because clients can always write
        FD_SET(it->first, &Desc._ER_fds);
    }
    _max_fd = _Clients.rbegin()->first + 1;

    _ready_FD = select(_max_fd, &Desc._READ_fds, &Desc._WR_fds, &Desc._ER_fds, &_timeout);

    // ready for the specified events
    if (_ready_FD == -1)
    {
       this->closeFreeALL();
       throw Server::Excp("Error :occurred during 'select' operation");
    }
    else if (_ready_FD)
    {
        struct sockaddr_in client_addr;
        client_addr.sin_family = AF_INET;
        client_addr.sin_port = htons(_port);
        socklen_t len = sizeof(client_addr);

        // Check if the listening socket is ready
        if (FD_ISSET(_server_fd, &Desc._READ_fds))
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
                if (fcntl(client_fd, F_SETFL, O_NONBLOCK) == -1)
                {
                    close(_server_fd);
                    throw Server::Excp("Error: setting client fd to non-blocking mode failed!");
                }

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
        // DEBUGGER();
        if (FD_ISSET(it->first, &Desc._READ_fds))
        {
            // DEBUGGER();
            sizeBuff = recv(it->first, buffer, sizeof(buffer), 0);
            // DEBUGGER();
            if (sizeBuff == -1)
            {
                FD_CLR(it->first, &this->Desc._READ_fds);
                it->second->setClosed(true);
                _ifSend = true;                                // Unexpected kick
                addRemoveFd(it->second);
                std::cout << "The client is disconnected Unexpected (fd = " << it->first<< ")." << std::endl;
            }
            else if (sizeBuff == 0)
            {
                FD_CLR(it->first, &this->Desc._READ_fds);
                it->second->setClosed(true);
                _command->commandQUIT(it->second);
                std::cout << "The client is disconnected (fd = " << it->first<< ")." << std::endl;
            }
            else
            {
                it->second->setClosed(_ifSend);
                int i = -1;
                while (buffer[++i])
                {
                    it->second->_tmpBuffer += buffer[i];
                }
                if (it->second->_tmpBuffer.find('\n') != std::string::npos)
                {
                    it->second->setInputBuffer(it->second->_tmpBuffer);
                    it->second->splitBufferToList();
                    it->second->setArguments();
                    while (!it->second->getArguments().empty() || !it->second->getCommand().empty())
                    {
                        _command->commandHandler(it->second);
                        it->second->setArguments();
                    }
                    it->second->_tmpBuffer = "";
                }
            }
        }

    }
    removefromMaps();
    _ifSend = false;
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

void Server::removefromMaps()
{
    Client* C;

    while(!_removedFDs.empty())
    {
        C = _removedFDs.top();
        std::map<std::string, int>::iterator it = _nickname.begin();
        for( ; it != _nickname.end(); ++it)
        {
            DEBUGGER();
            if (it->first == C->getNICK())
            {
                DEBUGGER();
                C->leavingALLChannelsUnexpected();
                _nickname.erase(it);
                break ;
            }
        }
        std::map<int, Client*>::iterator it1 = _Clients.begin();
        for(; it1 != _Clients.end();++it1)
        {
            if (it1->second == C)
            {
                DEBUGGER();
                close(it1->first);
                delete C;
                _Clients.erase(it1);
                break ;
            }
        }
        _removedFDs.pop();
    }
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
    closeFreeALL();
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
    {
        // DEBUGGER();
        return NULL;
    }
    return _Clients.find(it->second)->second;
}


//-----------------------------------------------------------               SET              ------------------


void Server::updateNickMap(Client* C, std::string &nick)  //   all maps add Client
{
// DEBUGGER();
    if (this->getClient(C->getNICK()))
        _nickname.erase(C->getNICK());
// DEBUGGER();
    _nickname[nick] = C->getFd();
}


void Server::checkForCloseCannel(void)
{
    std::set<Channel*>::iterator it = _channels.begin();
    for ( ; it != _channels.end(); ++it)
    {
        if ((*it)->emptyClients())
        {
            _channels.erase((*it));
            delete *it;
        }
    }
}


//----------------------------------------------      get  



Channel* Server::getChannel(std::string &name)
{
    std::set<Channel*>::iterator it = _channels.begin();
    for( ;it  != _channels.end(); ++it)
    {
        if ((*it)->getChannelName() == name)
        {
            return (*it);
        }
    }
    return NULL;
}


//-----------------------------------------------------


Channel *Server::createChannel(const std::string &name, const std::string &pass)
{
    Channel *chan = new Channel(name, pass);
    _channels.insert(chan);
    return chan;
}

void Server::addRemoveFd(Client* C)
{
    if (C)
        _removedFDs.push(C);
}
