#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <ctype.h>

int main() {
    size_t max_buffer_size = 1024;
    int port = 22394;

    int serverSocket;// socket descriptor for the server
    struct sockaddr_in server_addr, client_addr;
    char *buffer = malloc(max_buffer_size);
    int option;//Variable to store client's option
    if (!buffer) {// check if memory allocation failed
        perror("Failed to allocate memory for buffer");
        return -1;
    }

    
    socklen_t client_len = sizeof(client_addr);// size of the client address structure

    // Create socket
    if ((serverSocket = socket(AF_INET, SOCK_DGRAM, 0)) < 0) {
        perror("socket creation failed");
        free(buffer);
        return -1;
    }

    memset(&server_addr, 0, sizeof(server_addr));// initialize server address structure to zero
    memset(&client_addr, 0, sizeof(client_addr));// initialize client address structure to zero

    // Configure server address
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = inet_addr("127.0.0.1");
    server_addr.sin_port = htons(port);

    // Bind socket to server address
    if (bind(serverSocket, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0) {
        perror("bind failed");
        free(buffer);
        close(serverSocket);
        return -1;
    }


    while (1) {
        // Receive option from client
        ssize_t recv_len = recvfrom(serverSocket, &option, sizeof(int), 0, (struct sockaddr *)&client_addr, &client_len);
        if (recv_len < 0) {// check errors in receiving data
            perror("recvfrom failed");
            continue;
        }
        printf("Received option: %d\n", option);

        // Receive message from client
        recv_len = recvfrom(serverSocket, buffer, max_buffer_size - 1, 0, (struct sockaddr *)&client_addr, &client_len);
        if (recv_len < 0) {// check errors in receiving data
            perror("recvfrom failed");
            continue;
        }
        buffer[recv_len] = '\0';//null-terminate the received message
        printf("Received message: %s\n", buffer);

        // Process message
        int len = strlen(buffer);//get length of the received message
        int i, j;
        char temp;
        switch (option) {
            case 1: // Convert to uppercase
                for (i = 0; i < len; i++) {
                    buffer[i] = toupper(buffer[i]);
                }
                break;
            case 2: // Convert to lowercase
                for (i = 0; i < len; i++) {
                    buffer[i] = tolower(buffer[i]);
                }
                break;
            case 3: // Reverse the order
                for (i = 0, j = len - 1; i < j; i++, j--) {
                    temp = buffer[i];
                    buffer[i] = buffer[j];
                    buffer[j] = temp;
                }
                break;
        }

        // Send processed message back to client
        if (sendto(serverSocket, buffer, strlen(buffer), 0, (struct sockaddr *)&client_addr, client_len) < 0) {
            perror("sendto failed");
            continue;
        }
        printf("Reply message: %s\n\n", buffer);
    }

    free(buffer);
    close(serverSocket);
    return 0;
}

