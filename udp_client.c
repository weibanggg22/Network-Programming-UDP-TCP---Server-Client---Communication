#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>

int main() {
    int clientSocket, portNum, nBytes;
    struct sockaddr_in server_addr;
    char *buffer = malloc(1024); //allocate memory for receiving data
    if (buffer == NULL) {
        perror("Failed to allocate memory for buffer");
        exit(EXIT_FAILURE);
    }
    int option;
    char input[100];  // Buffer for input

    // create socket
    if ((clientSocket = socket(AF_INET, SOCK_DGRAM, 0)) < 0) { //checks for errors and exits if socket creation fails
        perror("socket creation failed");
        free(buffer);
        exit(EXIT_FAILURE);
    }

    memset(&server_addr, 0, sizeof(server_addr));

    // Configure server address
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(22394);
    server_addr.sin_addr.s_addr = inet_addr("127.0.0.1");

    while (1) {
        // print menu and get valid option
        while (1) {
            printf("=== Options ===\n");
            printf("1. Convert characters to uppercase letters.\n");
            printf("2. Convert characters to lowercase letters.\n");
            printf("3. Reverse the order of characters.\n");
            printf("4. Print the sockaddr_in structure.\n");
            printf("Your option: ");
            
            if (fgets(input, sizeof(input), stdin) == NULL) { // read user input
                printf("Error reading input. Please try again.\n");
                continue;
            }
            input[strcspn(input, "\n")] = 0;// remove newline character
            if (sscanf(input, "%d", &option) != 1) {// convert input to integer
                printf("Invalid input. Please enter a number between 1 and 4.\n\n");
                continue;
            }
            if (option < 1 || option > 4) {//validate option range
                printf("Invalid option. Please choose a number between 1 and 4.\n\n");
                continue;
            }
            break;
        }

        if (option == 4) {
            // print sockaddr_in structure
            printf("sockaddr_in structure:\n");
            printf("sin_family = %d\n", server_addr.sin_family);
            printf("sin_port = %d\n", ntohs(server_addr.sin_port));
            printf("sin_addr.s_addr = %s\n", inet_ntoa(server_addr.sin_addr));
            printf("\n");
            continue;
        }

        // send option to server
        if (sendto(clientSocket, &option, sizeof(int), 0, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0) {
            perror("sendto failed");
            continue;
        }

        printf("Your message: ");
        if (fgets(buffer, 1024, stdin) == NULL) {
            printf("Error reading message. Please try again.\n");
            continue;
        }
        buffer[strcspn(buffer, "\n")] = 0; // Remove newline

        // Send message to server
        if (sendto(clientSocket, buffer, strlen(buffer), 0, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0) {
            perror("sendto failed");
            continue;
        }

        printf("Input message: %s\n", buffer);

        // Receive response from server
        socklen_t server_len = sizeof(server_addr);
        ssize_t n = recvfrom(clientSocket, buffer, 1024 - 1, 0, (struct sockaddr *)&server_addr, &server_len);
        if (n < 0) {
            perror("recvfrom failed");//if fail display error
            continue;
        }
        buffer[n] = '\0';//null-terminate received data

        printf("Server reply: %s\n\n", buffer);
    }

    close(clientSocket);
    free(buffer);// clear allocated memory
    return 0;
}

