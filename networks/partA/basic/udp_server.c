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
    int serverSocket;
    struct sockaddr_in serverAddr;
    socklen_t addr_size;
    char buffer[MAX_BUFFER_SIZE];
    char response[MAX_BUFFER_SIZE]; // Buffer to store the response

    // Create socket
    serverSocket = socket(AF_INET, SOCK_DGRAM, 0);
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

    printf("Listening for UDP clients...\n");

    addr_size = sizeof(serverAddr);

    while (1) { // Continuous loop to receive and respond to client messages
        // Receive data from client
        recvfrom(serverSocket, buffer, MAX_BUFFER_SIZE, 0, (struct sockaddr*)&serverAddr, &addr_size);
        printf("Received from client: %s\n", buffer);

        // Prompt the server user to enter a response
        printf("Enter a response to send to the client (or type 'quit' to exit): ");
        fgets(response, MAX_BUFFER_SIZE, stdin);

        // Check if the server user wants to quit
        if (strcmp(response, "quit\n") == 0) {
            break; // Exit the loop and close the server
        }

        // Send response to client
        sendto(serverSocket, response, strlen(response), 0, (struct sockaddr*)&serverAddr, addr_size);
    }

    close(serverSocket);

    return 0;
}
