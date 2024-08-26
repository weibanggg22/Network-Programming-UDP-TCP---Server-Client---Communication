#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <errno.h>

int main() {
    const int MAX_MESSAGE_LENGTH = 1024; //set maximum length for message

    int ClientSock = 0; //socket descriptor
    struct sockaddr_in serv_addr; //structure to store server address information
    char message[MAX_MESSAGE_LENGTH];
    char buffer[MAX_MESSAGE_LENGTH];

    // create socket
    if ((ClientSock = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        perror("Socket creation error");//check error
        return -1;
    }

    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(22394);

    // Convert IPv4 from text to binary form
    if (inet_pton(AF_INET, "127.0.0.1", &serv_addr.sin_addr) <= 0) {
        perror("Invalid address/ Address not supported");
        return -1;
    }

    // Connect to server
    if (connect(ClientSock, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0) {
        perror("Connection Failed");// check error connection
        return -1;
    }

    while (1) {
        printf("Message: ");
        fgets(message, MAX_MESSAGE_LENGTH, stdin);// read the input
        message[strcspn(message, "\n")] = 0;  // remove newline
        
        if (strlen(message) == 0) {// if error input
            printf("Error: Empty input. Please type a message.\n");
            continue;  // Prompt for input again
        }

        send(ClientSock, message, strlen(message), 0);// send message to server

        int valread = read(ClientSock, buffer, MAX_MESSAGE_LENGTH);// read response from server
        if (valread > 0) {
            buffer[valread] = '\0';  // null-terminate the buffer
            printf("Server reply: %s\n", buffer);
        } else {// if read fail then will print out error message
            printf("Server closed connection or read error\n");
            break;
        }
        printf("\n");
        memset(buffer, 0, sizeof(buffer));// clear the buffer for the next message
    }

    close(ClientSock);
    return 0;
}

