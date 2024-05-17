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
    int clientSocket;
    struct sockaddr_in serverAddr;
    char buffer[MAX_BUFFER_SIZE];
    char userInput[MAX_BUFFER_SIZE]; // Buffer to store user input

    // Create socket
    clientSocket = socket(AF_INET, SOCK_STREAM, 0);
    if (clientSocket < 0) {
        perror("Error in socket creation");
        exit(1);
    }

    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(PORT);
    serverAddr.sin_addr.s_addr = inet_addr("127.0.0.1");

    // Connect to server
    if (connect(clientSocket, (struct sockaddr*)&serverAddr, sizeof(serverAddr)) < 0) {
        perror("Error in connection");
        exit(1);
    }

    while (1) { // Loop to send multiple messages
        printf("YOU(client)(type 'quit' to exit): ");
        fgets(userInput, MAX_BUFFER_SIZE, stdin);

        // Check if the user wants to quit
        if (strcmp(userInput, "quit\n") == 0) {
            break; // Exit the loop and close the client
        }

        // Send user input to the server
        send(clientSocket, userInput, strlen(userInput), 0);

        // Receive response from server
        recv(clientSocket, buffer, MAX_BUFFER_SIZE, 0);
        printf("Server: %s\n", buffer);
    }

    close(clientSocket);

    return 0;
}
