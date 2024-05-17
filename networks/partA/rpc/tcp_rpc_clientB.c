#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>

int main() {
    char *ip = "127.0.0.1";
    int port = 12346;

    int sock;
    struct sockaddr_in addr;
    socklen_t addr_size;
    char buffer[1024];
    int n;

    sock = socket(AF_INET, SOCK_STREAM, 0);
    if (sock < 0) {
        perror("[-]Socket error");
        exit(1);
    }
    printf("[+]TCP client socket created.\n");

    memset(&addr, '\0', sizeof(addr));
    addr.sin_family = AF_INET;
    addr.sin_port = port;
    addr.sin_addr.s_addr = inet_addr(ip);

    if (connect(sock, (struct sockaddr*)&addr, sizeof(addr)) < 0) {
        perror("[-]Connection error");
        exit(1);
    }
    printf("Connected to the server.\n");

    while (1) {
        char choice[10];
        printf("Enter your choice (rock, paper, or scissors or): ");
        fgets(choice, sizeof(choice), stdin);
        choice[strlen(choice) - 1] = '\0'; // Remove the newline character

        // Send the player's choice to the server
        send(sock, choice, strlen(choice), 0);

        if (strcmp(choice, "quit") == 0) {
            printf("Exiting the game.\n");
            break;
        }

        bzero(buffer, 1024);
        recv(sock, buffer, sizeof(buffer), 0);
        printf("Server: %s\n", buffer);
         
        char cont[10]; 
        printf("Do u want to play again(yes/no):");
        fgets(cont,sizeof(cont),stdin);
        send(sock,cont,sizeof(cont),0);
        char cont1[10];
        recv(sock,cont1,sizeof(cont1),0);
        if(strcmp(cont,"no\n")==0 || strcmp(cont1,"no\n")==0)
        {
            break;
        }
    }

    close(sock);
    printf("Disconnected from the server.\n");

    return 0;
}
