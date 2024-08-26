#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>

int main() {
    const int MAX_MESSAGE_LENGTH = 1024; //maximum message buffer size

    int helloSocket, newSocket; //socket description
    struct sockaddr_in address; //structure for server address
    int opt = 1; //option for setsockopt
    int addrlen = sizeof(address); //length of address structure
    char buffer[MAX_MESSAGE_LENGTH];

    // creating socket file descriptor
    if ((helloSocket = socket(AF_INET, SOCK_STREAM, 0)) == 0) {
        perror("socket failed");
        exit(EXIT_FAILURE);
    }

    // forcefully attaching socket to the port
    if (setsockopt(helloSocket, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &opt, sizeof(opt))) {
        perror("setsockopt");
        exit(EXIT_FAILURE);
    }
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = inet_addr("127.0.0.1");
    address.sin_port = htons(22394); //port number

    // binding socket to the port and IP address
    if (bind(helloSocket, (struct sockaddr *)&address, sizeof(address)) < 0) { //check binding
        perror("bind failed");
        exit(EXIT_FAILURE);
    }

    printf("Bind success\n");
    
    // putting the server socket in a passive mode to listen for incoming connections
    if (listen(helloSocket, 3) < 0) {
        perror("listen");
        exit(EXIT_FAILURE);
    }

    printf("Listening\n");

    while(1) {
    // accepting incoming connection from a client
        if ((newSocket = accept(helloSocket, (struct sockaddr *)&address, (socklen_t*)&addrlen)) < 0) { // if read returns 0 or less connection is closed or error occurred
            perror("accept");
            exit(EXIT_FAILURE);
        }

        while(1) {
            //read message from client
            int valread = read(newSocket, buffer, MAX_MESSAGE_LENGTH - 1);
            if (valread <= 0) {
                break;
            }
            buffer[valread] = '\0';  //null-terminate the buffer
            printf("Received message: %s\n", buffer);
            //send the message back to client
            send(newSocket, buffer, strlen(buffer), 0);
            printf("The reply is sent.\n");

            memset(buffer, 0, sizeof(buffer)); // clear the buffer for the next message
        }
        close(newSocket);
    }

    return 0;
}

