#include <iostream>
#include <cstring>
#include <unistd.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <arpa/inet.h>
#include <netdb.h>

int main() {
    // Create socket
    int listening = socket(AF_INET, SOCK_STREAM, 0);
    if (listening == -1) {
        std::cerr << "Error creating socket" << std::endl;
        return 1;
    }

    // Set up server address
    struct sockaddr_in hint;
    hint.sin_family = AF_INET;
    hint.sin_port = htons(12345); // Port number
    hint.sin_addr.s_addr = INADDR_ANY; // Accept connections from any IP address
/*
// Forcefully attaching socket to the port 8080
// Ստիպողաբար միացնելով վարդակը 8080 պորտին
//int opt = 1;
//setsockopt(listening, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));
*/


   inet_pton(AF_INET, "0.0.0.0", &hint.sin_addr);
    

    if (bind(listening, (struct sockaddr*)&hint, sizeof(hint)) == -1) {
        std::cerr << "Error binding socket" << std::endl;
        close(listening);
        return 1;
    }
    // Listen for incoming connections
    if (listen(listening, SOMAXCONN) == -1) {
        std::cerr << "Error listening" << std::endl;
        close(listening);
        return 1;
    }

    std::cout << "Server listening on port 12345..." << std::endl;

    // Accept and handle incoming connections
    struct sockaddr_in clientAddr;
    socklen_t clientAddrSize = sizeof(clientAddr);
    char host[NI_MAXHOST];
    char svc[NI_MAXSERV];

    int clientSocket = accept(listening, (struct sockaddr*)&clientAddr,
               &clientAddrSize);
    if (clientSocket == -1) {
        std::cerr << "Error accepting connection" << std::endl;
        close(listening);
        return 1;
    }
    close(listening);

    memset(host, 0, NI_MAXHOST);
    memset(svc, 0, NI_MAXSERV);


//see if i can get the name of the computer
    int result = getnameinfo((sockaddr*)&clientAddr,
                    sizeof(clientAddr),
                    host,
                    NI_MAXHOST,
                    svc,
                    NI_MAXSERV,
                    0);

    if (result)
    {
        std::cout<< host << " connected on "<< svc<<std::endl; //???server
    }
    else
    {
        inet_ntop(AF_INET, &clientAddr.sin_addr, host, NI_MAXHOST);
        std::cout<< host << " connected on " 
            << ntohs(clientAddr.sin_port)<<std::endl;
    }
    // Receive data from client
    char buffer[4096];
    while(true)
    {
        //clear the buffer
        memset(buffer, 0, 4096);
        //wait for a message
        int bytesRecv = recv(clientSocket, buffer, sizeof(buffer), 0);
        if (bytesRecv == -1)
        {
            std::cerr<<"There was a connection issue"<<std::endl;
            break;
        }
        if (bytesRecv == 0)
        {
            std::cerr<<"There client disconnected"<<std::endl;
            break;
        }
        //display message
        std::cout << "Received message from client:buffer==  : " << buffer 
            << " : std::string(buffer, 0, bytesRecv) <" 
                << std::string(buffer, 0, bytesRecv) <<"> " <<std::endl;
        // resend message
        send(clientSocket, buffer, bytesRecv + 1, 0);
        
        


    }
    // Close sockets
    close(clientSocket);

    return 0;
}
