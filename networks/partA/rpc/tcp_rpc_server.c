#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>

int main()
{
    char *ip = "127.0.0.1";
    int port = 12345;

    int server_sock, clientA_socket;
    struct sockaddr_in server_addr, client_addr;
    socklen_t addr_size;
    char bufferA[1024];
    int n;

    server_sock = socket(AF_INET, SOCK_STREAM, 0);
    if (server_sock < 0)
    {
        perror("[-]Socket error");
        exit(1);
    }
    printf("[+]TCP server socket created.\n");

    memset(&server_addr, '\0', sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = port;
    server_addr.sin_addr.s_addr = inet_addr(ip);

    n = bind(server_sock, (struct sockaddr *)&server_addr, sizeof(server_addr));
    if (n < 0)
    {
        perror("[-]Bind error");
        exit(1);
    }
    printf("[+]Bind to the port number: %d\n", port);

    // 2nd part
    int port1 = 12346;
    int server_sock1, clientB_socket;
    struct sockaddr_in server_addr1, client_addr1;
    socklen_t addr_size1;
    char bufferB[1024];
    int n1;

    server_sock1 = socket(AF_INET, SOCK_STREAM, 0);
    if (server_sock1 < 0)
    {
        perror("[-]Socket error");
        exit(1);
    }
    printf("[+]TCP server socket created.\n");

    memset(&server_addr1, '\0', sizeof(server_addr1));
    server_addr1.sin_family = AF_INET;
    server_addr1.sin_port = port1;
    server_addr1.sin_addr.s_addr = inet_addr(ip);

    n1 = bind(server_sock1, (struct sockaddr *)&server_addr1, sizeof(server_addr1));
    if (n1 < 0)
    {
        perror("[-]Bind error");
        exit(1);
    }
    printf("[+]Bind to the port number: %d\n", port1);

    listen(server_sock, 5);
    listen(server_sock1, 5);

    addr_size = sizeof(client_addr);
    clientA_socket = accept(server_sock, (struct sockaddr *)&client_addr, &addr_size);
    printf("[+]Client A connected to the server.\n");

    printf("Listening...\n");

    addr_size1 = sizeof(client_addr1);
    clientB_socket = accept(server_sock1, (struct sockaddr *)&client_addr1, &addr_size1);

    printf("[+]Client B connected to the server.\n");
    printf("Listening...\n");

    while (1)
    {
        // Receive decisions from clientA and clientB
        recv(clientA_socket, bufferA, sizeof(bufferA), 0);
        recv(clientB_socket, bufferB, sizeof(bufferB), 0);

        // Your logic to determine the result goes here
        // For simplicity, let's assume Rock (0), Paper (1), and Scissors (2)
        int decisionA = atoi(bufferA);
        int decisionB = atoi(bufferB);

        char resultA[10], resultB[10];

        if (decisionA == decisionB)
        {
            strcpy(resultA, "Draw");
            strcpy(resultB, "Draw");
        }
        else if ((decisionA == 0 && decisionB == 2) || (decisionA == 1 && decisionB == 0) || (decisionA == 2 && decisionB == 1))
        {
            strcpy(resultA, "Win");
            strcpy(resultB, "Lost");
        }
        else
        {
            strcpy(resultA, "Lost");
            strcpy(resultB, "Win");
        }

        // Send the result to both clients
        send(clientA_socket, resultA, sizeof(resultA), 0);
        send(clientB_socket, resultB, sizeof(resultB), 0);

        // Prompt for another game
        char playAgain1[10];
        char playAgain2[10];

        recv(clientA_socket, playAgain1, sizeof(playAgain1), 0);
        recv(clientB_socket, playAgain2, sizeof(playAgain2), 0);
        send(clientA_socket, playAgain2, sizeof(playAgain2), 0);
        send(clientB_socket, playAgain1, sizeof(playAgain1), 0);

        // printf("%s\n", playAgain1);
        // printf("%s\n", playAgain2);
        if (strcmp(playAgain1, "no\n") == 0 && strcmp(playAgain2, "no\n") == 0)
        {
            printf("Game over. Exiting...\n");
            break;
        }
    }

    // Close sockets
    close(clientA_socket);
    close(clientB_socket);
    close(server_sock);
    close(server_sock1);

    return 0;
}
