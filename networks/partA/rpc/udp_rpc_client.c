#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <arpa/inet.h>
#include <sys/socket.h>

#define SERVER_IP "127.0.0.1"
#define PORT 12345

int main() {
    // Create UDP socket
    int client_socket;
    if ((client_socket = socket(AF_INET, SOCK_DGRAM, 0)) == -1) {
        perror("Socket creation failed");
        exit(1);
    }

    // Server address configuration
    struct sockaddr_in server_addr;
    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(PORT);
    server_addr.sin_addr.s_addr = inet_addr(SERVER_IP);

    char decision[256];

    while (1) {
        // Get user's decision
        printf("Enter your decision (0 for Rock, 1 for Paper, 2 for Scissors): ");
        scanf("%s", decision);

        // Send decision to the other player (clientB)
        sendto(client_socket, decision, strlen(decision), 0, (struct sockaddr *)&server_addr, sizeof(server_addr));

        // Receive decision from the other player (clientA)
        char other_player_decision[256];
        socklen_t server_len = sizeof(server_addr);
        int bytes_received = recvfrom(client_socket, other_player_decision, sizeof(other_player_decision), 0, (struct sockaddr *)&server_addr, &server_len);

        if (bytes_received == -1) {
            perror("Error receiving other player's decision");
            exit(1);
        }

        // Display both players' decisions
        printf("Your decision: %s\n", decision);
        printf("Other player's decision: %s\n", other_player_decision);

        // Determine the result
        int result;
        if (strcmp(decision, other_player_decision) == 0) {
            result = 0; // Draw
        } else if ((strcmp(decision, "0") == 0 && strcmp(other_player_decision, "2") == 0) ||
                   (strcmp(decision, "1") == 0 && strcmp(other_player_decision, "0") == 0) ||
                   (strcmp(decision, "2") == 0 && strcmp(other_player_decision, "1") == 0)) {
            result = 1; // You win
        } else {
            result = 2; // You lose
        }

        // Display the result
        if (result == 0) {
            printf("It's a draw!\n");
        } else if (result == 1) {
            printf("You win!\n");
        } else {
            printf("You lose!\n");
        }

        // Prompt for another game
        char play_again;
        printf("Do you want to play again? (y/n): ");
        scanf(" %c", &play_again);

        if (play_again != 'y' && play_again != 'Y') {
            break;
        }
    }

    close(client_socket);
    return 0;
}
