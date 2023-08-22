// #include <iostream>
// #include <cstring>
// #include <unistd.h>
// #include <sys/socket.h>
// #include <sys/types.h>
// #include <arpa/inet.h> // Add this header for 'inet_addr'
// #include <netdb.h>

// int main() {
//     // Create socket
//     int clientSocket = socket(AF_INET, SOCK_STREAM, 0);
//     if (clientSocket == -1) {
//         std::cerr << "Error creating socket" << std::endl;
//         return 1;
//     }

//     // Set up server address
//     struct sockaddr_in serverAddr;
//     serverAddr.sin_family = AF_INET;
//     serverAddr.sin_port = htons(12345); // Port number
//     serverAddr.sin_addr.s_addr = inet_addr("127.0.0.1"); // Server IP address

//     // Connect to server
//     if (connect(clientSocket, (struct sockaddr*)&serverAddr, sizeof(serverAddr)) == -1) {
//         std::cerr << "Error connecting to server" << std::endl;
//         close(clientSocket);
//         return 1;
//     }

//     // Send data to server
//     const char* message = "Hello from client!";
//     send(clientSocket, message, strlen(message), 0);

//     // Receive response from server
//     char buffer[1024];
//     int bytesRead = recv(clientSocket, buffer, sizeof(buffer), 0);
//     if (bytesRead > 0) {
//         std::cout << "Received response from server: " << buffer << std::endl;
//     }

//     // Close socket
//     close(clientSocket);

//     return 0;
// }
