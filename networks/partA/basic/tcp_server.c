#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <sys/socket.h>

#define PORT 12345
#define MAX_BUFFER_SIZE 1024

int main() {
    int serverSocket, newSocket;
    struct sockaddr_in serverAddr, newAddr;
    socklen_t addr_size;
    char buffer[MAX_BUFFER_SIZE];

    // Create socket
    serverSocket = socket(AF_INET, SOCK_STREAM, 0);
    if (serverSocket < 0) {
        perror("Error in socket creation");
        exit(1);
    }

    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(PORT);
    serverAddr.sin_addr.s_addr = INADDR_ANY;

    // Bind socket
    if (bind(serverSocket, (struct sockaddr*)&serverAddr, sizeof(serverAddr)) < 0) {
        perror("Error in binding");
        exit(1);
    }

    // Listen for clients
    if (listen(serverSocket, 10) == 0) {
        printf("Listening...\n");
    } else {
        perror("Error in listening");
        exit(1);
    }

    while (1) { // Continuous loop to accept connections
        addr_size = sizeof(newAddr);
        newSocket = accept(serverSocket, (struct sockaddr*)&newAddr, &addr_size); // Accept connection from client

        while (1) { // Loop to handle messages from one client
            // Receive data from client
            ssize_t recvBytes = recv(newSocket, buffer, MAX_BUFFER_SIZE, 0);
            if (recvBytes <= 0) {
                // Client disconnected or an error occurred
                break;
            }

            buffer[recvBytes] = '\0'; // Null-terminate received data
            printf("Client: %s\n", buffer);

            // Prompt the server user to enter a response
            printf("YOU(server)(type 'quit' to exit): ");
            fgets(buffer, MAX_BUFFER_SIZE, stdin); // Read user input for response

            // Check if the server user wants to quit
            if (strcmp(buffer, "quit\n") == 0) {
                break; // Exit the loop and close the client
            }

            // Send response to client
            send(newSocket, buffer, strlen(buffer), 0);
        }

        close(newSocket); // Close the client socket
    }

    close(serverSocket);

    return 0;
}
