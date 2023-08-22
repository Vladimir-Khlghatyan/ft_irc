// #include <stdio.h>
// #include <stdlib.h>
// #include <string.h>
// #include <unistd.h>
// #include <sys/types.h>
// #include <sys/socket.h>
// #include <netinet/in.h>
// #include <arpa/inet.h>

// int main() {
//     int listen_fd, client_fd;
//     struct sockaddr_in server_addr, client_addr;
//     socklen_t client_addr_len = sizeof(client_addr);

//     // Create a listening socket
//     listen_fd = socket(AF_INET, SOCK_STREAM, 0);
//     if (listen_fd == -1) {
//         perror("socket");
//         return 1;
//     }

//     // Bind the socket to a specific address and port
//     memset(&server_addr, 0, sizeof(server_addr));
//     server_addr.sin_family = AF_INET;
//     server_addr.sin_addr.s_addr = INADDR_ANY;
//     server_addr.sin_port = htons(12345);

//     if (bind(listen_fd, (struct sockaddr *)&server_addr, sizeof(server_addr)) == -1) {
//         perror("bind");
//         close(listen_fd);
//         return 1;
//     }

//     // Listen for incoming connections
//     if (listen(listen_fd, 5) == -1) {
//         perror("listen");
//         close(listen_fd);
//         return 1;
//     }

//     fd_set read_fds;
//     int max_fd = listen_fd + 1;

//     while (1) {
//         // Initialize and set the file descriptor set
//             FD_ZERO(&read_fds);
//             FD_SET(listen_fd, &read_fds);

//         // Wait for events or a timeout of 5 seconds
//         struct timeval timeout;
//         timeout.tv_sec = 5;
//         timeout.tv_usec = 0;

//         int ready_fds = select(max_fd, &read_fds, NULL, NULL, &timeout);
//         if (ready_fds == -1) {
//             perror("select");
//             break;
//         }

//         if (ready_fds == 0) {
//             printf("No activity within 5 seconds.\n");
//             continue;
//         }

//         // Check if the listening socket is ready
//         if (FD_ISSET(listen_fd, &read_fds)) {
//             client_fd = accept(listen_fd, (struct sockaddr *)&client_addr, &client_addr_len);
//             if (client_fd == -1) {
//                 perror("accept");
//                 continue;
//             }

//             printf("New connection from %s:%d\n", inet_ntoa(client_addr.sin_addr), ntohs(client_addr.sin_port));
            
//             // Add the new client_fd to the set
//             FD_SET(client_fd, &read_fds);

//             if (client_fd >= max_fd) {
//                 max_fd = client_fd + 1;
//             }
//         }

//         // Check other file descriptors for reading

//         for (int fd = listen_fd + 1; fd < max_fd; ++fd) {
//             if (FD_ISSET(fd, &read_fds)) {
//                 // Handle data read from the client
//                 char buffer[1024];
//                 ssize_t bytes_read = read(fd, buffer, sizeof(buffer));
//                 if (bytes_read == -1) {
//                     perror("read");
//                     close(fd);
//                     FD_CLR(fd, &read_fds);
//                 } else if (bytes_read == 0) {
//                     printf("Connection closed by client.\n");
//                     close(fd);
//                     FD_CLR(fd, &read_fds);
//                 } else {
//                     printf("Received: %.*s", (int)bytes_read, buffer);
//                 }
//             }
//         }
//     }

//     close(listen_fd);
//     return 0;
// }