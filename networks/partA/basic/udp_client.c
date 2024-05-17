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
    socklen_t addr_size;
    char buffer[MAX_BUFFER_SIZE];
    char userInput[MAX_BUFFER_SIZE]; // Buffer to store user input

    // Create socket
    clientSocket = socket(AF_INET, SOCK_DGRAM, 0);
    if (clientSocket < 0) {
        perror("Error in socket creation");
        exit(1);
    }

    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(PORT);
    serverAddr.sin_addr.s_addr = inet_addr("127.0.0.1");

    addr_size = sizeof(serverAddr);

    while (1) { // Loop to send multiple messages
        printf("Enter a message to send to the server (or type 'quit' to exit): ");
        fgets(userInput, MAX_BUFFER_SIZE, stdin);

        // Check if the user wants to quit
        if (strcmp(userInput, "quit\n") == 0) {
            break; // Exit the loop and close the client
        }

        // Send user input to the server
        sendto(clientSocket, userInput, strlen(userInput), 0, (struct sockaddr*)&serverAddr, addr_size);

        // Receive response from server
        recvfrom(clientSocket, buffer, MAX_BUFFER_SIZE, 0, NULL, NULL);
        printf("Received from server: %s\n", buffer);
    }

    close(clientSocket);

    return 0;
}
