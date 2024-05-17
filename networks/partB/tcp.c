#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <time.h>

#define SERVER_IP "127.0.0.1"
#define SERVER_PORT 12345
#define CHUNK_SIZE 4

struct PacketHeader
{
    int sequence_number;
    int total_chunks;
};

void create_chunk(char *chunk, int sequence_number, int total_chunks, const char *data)
{
    struct PacketHeader header;
    header.sequence_number = sequence_number;
    header.total_chunks = total_chunks;
    memcpy(chunk, &header, sizeof(struct PacketHeader));
    memcpy(chunk + sizeof(struct PacketHeader), data, CHUNK_SIZE);
}

void extract_chunk_info(const char *chunk, int *sequence_number, int *total_chunks, char *data)
{
    struct PacketHeader header;
    memcpy(&header, chunk, sizeof(struct PacketHeader));
    *sequence_number = header.sequence_number;
    *total_chunks = header.total_chunks;
    memcpy(data, chunk + sizeof(struct PacketHeader), CHUNK_SIZE);
}

void send_with_random_loss(int sock, const char *packet, size_t packet_size, struct sockaddr_in *server_addr)
{
    // Simulate packet loss by randomly dropping ACK packets
    if (rand() / (double)RAND_MAX > 0.3)
    {
        sendto(sock, packet, packet_size, 0, (struct sockaddr *)server_addr, sizeof(struct sockaddr_in));
    }
}

void server(int n)
{
    if (n == 0)
    {
        int sock = socket(AF_INET, SOCK_DGRAM, 0);
        struct sockaddr_in server_addr, client_addr;
        socklen_t addr_len = sizeof(struct sockaddr_in);

        server_addr.sin_family = AF_INET;
        server_addr.sin_port = htons(SERVER_PORT);
        inet_pton(AF_INET, SERVER_IP, &(server_addr.sin_addr));

        bind(sock, (struct sockaddr *)&server_addr, sizeof(struct sockaddr_in));

        int expected_sequence = 0;
        int total_chunks;
        char received_chunks[100][CHUNK_SIZE];
        memset(received_chunks, 0, sizeof(received_chunks));

        while (1)
        {
            char packet[100];
            ssize_t packet_size = recvfrom(sock, packet, sizeof(packet), 0, (struct sockaddr *)&client_addr, &addr_len);

            int packet_sequence, packet_total_chunks;
            char data[CHUNK_SIZE];
            extract_chunk_info(packet, &packet_sequence, &packet_total_chunks, data);

            if (packet_sequence == expected_sequence)
            {
                strncpy(received_chunks[packet_sequence], data, CHUNK_SIZE);
                expected_sequence++;

                // Send ACK for the received packet
                char ack_packet[sizeof(int)];
                memcpy(ack_packet, &packet_sequence, sizeof(int));
                send_with_random_loss(sock, ack_packet, sizeof(int), &client_addr);

                // Check if we've received all chunks
                if (expected_sequence == packet_total_chunks)
                {
                    // printf("Received Message: ");
                    // for (int i = 0; i < packet_total_chunks; i++) {
                    //     printf("%s", received_chunks[i]);
                    // }
                    // printf("\n");
                    break;
                }
            }
            else
            {
                // Send an ACK for the last successfully received packet
                char ack_packet[sizeof(int)];
                memcpy(ack_packet, &expected_sequence - 1, sizeof(int));
                send_with_random_loss(sock, ack_packet, sizeof(int), &client_addr);
            }
        }

        close(sock);
    }
    else
    {
        int sock = socket(AF_INET, SOCK_DGRAM, 0);
        struct sockaddr_in server_addr;

        server_addr.sin_family = AF_INET;
        server_addr.sin_port = htons(SERVER_PORT);
        inet_pton(AF_INET, SERVER_IP, &(server_addr.sin_addr));

        // char *data;
        // printf("Enter data to be sent to the client:");
        // fgets(data,sizeof(data),stdin);
        const char *data = "This is a message sent from the server to client.";
        int total_chunks = (strlen(data) + CHUNK_SIZE - 1) / CHUNK_SIZE;

        for (int i = 0; i < total_chunks; i++)
        {
            char chunk[sizeof(struct PacketHeader) + CHUNK_SIZE];
            create_chunk(chunk, i, total_chunks, data + i * CHUNK_SIZE);

            // Print the chunk before sending
            printf("Sending Chunk %d: %.*s\n", i, CHUNK_SIZE, data + i * CHUNK_SIZE);

            sendto(sock, chunk, sizeof(chunk), 0, (struct sockaddr *)&server_addr, sizeof(struct sockaddr_in));
            printf("Sent packet %d\n", i);

            // Simulate retransmissions for every third packet (comment out in final submission)
            if (i % 3 == 0)
            {
                usleep(200000);
                sendto(sock, chunk, sizeof(chunk), 0, (struct sockaddr *)&server_addr, sizeof(struct sockaddr_in));
                printf("Resent packet %d\n", i);
            }
        }

        close(sock);
    }
}

void client(int n)
{
    if (n == 0)
    {
        int sock = socket(AF_INET, SOCK_DGRAM, 0);
        struct sockaddr_in server_addr;

        server_addr.sin_family = AF_INET;
        server_addr.sin_port = htons(SERVER_PORT);
        inet_pton(AF_INET, SERVER_IP, &(server_addr.sin_addr));

        // char *data;
        // printf("Enter data to be sent to the server:");
        // fgets(data,sizeof(data),stdin);

        const char *data = "This is a message sent from the client to server.";

        int total_chunks = (strlen(data) + CHUNK_SIZE - 1) / CHUNK_SIZE;

        for (int i = 0; i < total_chunks; i++)
        {
            char chunk[sizeof(struct PacketHeader) + CHUNK_SIZE];
            create_chunk(chunk, i, total_chunks, data + i * CHUNK_SIZE);

            // Print the chunk before sending
            printf("Sending Chunk %d: %.*s\n", i, CHUNK_SIZE, data + i * CHUNK_SIZE);

            sendto(sock, chunk, sizeof(chunk), 0, (struct sockaddr *)&server_addr, sizeof(struct sockaddr_in));
            printf("Sent packet %d\n", i);

            // Simulate retransmissions for every third packet (comment out in final submission)
            if (i % 3 == 0)
            {
                usleep(200000);
                sendto(sock, chunk, sizeof(chunk), 0, (struct sockaddr *)&server_addr, sizeof(struct sockaddr_in));
                printf("Resent packet %d\n", i);
            }
        }

        close(sock);
    }
    else
    {
        int sock = socket(AF_INET, SOCK_DGRAM, 0);
        struct sockaddr_in server_addr, client_addr;
        socklen_t addr_len = sizeof(struct sockaddr_in);

        server_addr.sin_family = AF_INET;
        server_addr.sin_port = htons(SERVER_PORT);
        inet_pton(AF_INET, SERVER_IP, &(server_addr.sin_addr));

        bind(sock, (struct sockaddr *)&server_addr, sizeof(struct sockaddr_in));

        int expected_sequence = 0;
        int total_chunks;
        char received_chunks[100][CHUNK_SIZE];
        memset(received_chunks, 0, sizeof(received_chunks));

        while (1)
        {
            char packet[100];
            ssize_t packet_size = recvfrom(sock, packet, sizeof(packet), 0, (struct sockaddr *)&client_addr, &addr_len);

            int packet_sequence, packet_total_chunks;
            char data[CHUNK_SIZE];
            extract_chunk_info(packet, &packet_sequence, &packet_total_chunks, data);

            if (packet_sequence == expected_sequence)
            {
                strncpy(received_chunks[packet_sequence], data, CHUNK_SIZE);
                expected_sequence++;

                // Send ACK for the received packet
                char ack_packet[sizeof(int)];
                memcpy(ack_packet, &packet_sequence, sizeof(int));
                send_with_random_loss(sock, ack_packet, sizeof(int), &client_addr);

                // Check if we've received all chunks
                if (expected_sequence == packet_total_chunks)
                {
                    // printf("Received Message: ");
                    // for (int i = 0; i < packet_total_chunks; i++) {
                    //     printf("%s", received_chunks[i]);
                    // }
                    // printf("\n");
                    break;
                }
            }
            else
            {
                // Send an ACK for the last successfully received packet
                char ack_packet[sizeof(int)];
                memcpy(ack_packet, &expected_sequence - 1, sizeof(int));
                send_with_random_loss(sock, ack_packet, sizeof(int), &client_addr);
            }
        }

        close(sock);
    }
}

int main()
{
    int n;

    printf("In which direction do u want to send data: client to server(0) or server to client(1)\n");
    scanf("%d", &n);

    srand(time(NULL));

    
    if (n == 0)
    {
        pid_t pid = fork();
        if (pid == 0)
        {
            // Child process - Server
            server(n);
        }
        else if (pid > 0)
        {
            // Parent process - Client
            client(n);
        }
        else
        {
            perror("Fork failed");
            return 1;
        }
    }
    else
    {
        pid_t pid = fork();
        if (pid == 0)
        {
            // Child process - Server
            client(n);
        }
        else if (pid > 0)
        {
            // Parent process - Client
            server(n);
        }
        else
        {
            perror("Fork failed");
            return 1;
        }
    }

    return 0;
}
