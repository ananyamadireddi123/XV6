#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <arpa/inet.h>
#include <sys/socket.h>

#define PORT 12345

int main() {
    // Create UDP socket
    int server_socket;
    if ((server_socket = socket(AF_INET, SOCK_DGRAM, 0)) == -1) {
        perror("Socket creation failed");
        exit(1);
    }

    // Server address configuration
    struct sockaddr_in server_addr;
    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(PORT);
    server_addr.sin_addr.s_addr = INADDR_ANY;

    // Bind socket to server address
    if (bind(server_socket, (struct sockaddr *)&server_addr, sizeof(server_addr)) == -1) {
        perror("Binding failed");
        exit(1);
    }

    printf("Server listening on port %d...\n", PORT);

    struct sockaddr_in clientA_addr, clientB_addr;
    socklen_t clientA_len = sizeof(clientA_addr), clientB_len = sizeof(clientB_addr);

    while (1) {
        char decisionA[256], decisionB[256];

        // Receive decision from clientA
        int bytes_received_A = recvfrom(server_socket, decisionA, sizeof(decisionA), 0, (struct sockaddr *)&clientA_addr, &clientA_len);
        decisionA[bytes_received_A] = '\0';

        // Receive decision from clientB
        int bytes_received_B = recvfrom(server_socket, decisionB, sizeof(decisionB), 0, (struct sockaddr *)&clientB_addr, &clientB_len);
        decisionB[bytes_received_B] = '\0';

        // Send each player's decision to the other player
        sendto(server_socket, decisionB, strlen(decisionB), 0, (struct sockaddr *)&clientA_addr, clientA_len);
        sendto(server_socket, decisionA, strlen(decisionA), 0, (struct sockaddr *)&clientB_addr, clientB_len);
    }

    close(server_socket);
    return 0;
}
