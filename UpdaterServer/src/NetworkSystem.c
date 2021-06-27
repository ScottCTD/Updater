//
// Created by Scott on 2021/6/26.
//

#include "NetworkSystem.h"

int initSocketOnServer(unsigned short serverPort) {
    int serverSocket;
    int value;
    // Get our host address info.
    struct addrinfo hints, *serverInfo;
    memset(&hints, 0, sizeof hints);
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_flags = AI_PASSIVE;
    char port[6];
    sprintf(port, "%d", htons(serverPort));
    if ((value = getaddrinfo(NULL, port, &hints, &serverInfo)) != 0) {
        fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(value));
        return -1;
    }

    // Create and bind our server socket file descriptor.
    for (struct addrinfo *i = serverInfo; i != NULL; i = i->ai_next) {
        if ((serverSocket = socket(i->ai_family, i->ai_socktype, i->ai_protocol)) == -1) {
            perror("socket");
            continue;
        }
        if (bind(serverSocket, i->ai_addr, i->ai_addrlen) == -1) {
            close(serverSocket);
            perror("bind");
            continue;
        }
        break;
    }
    struct sockaddr_in *serverAddress = (struct sockaddr_in *) serverInfo->ai_addr;
    freeaddrinfo(serverInfo);

    // Listen for incoming client
    if (listen(serverSocket, BACKLOG) == -1) {
        perror("listen");
        return -1;
    }
    printf("Server start listening for clients on port %d!\n", serverAddress->sin_port);
    return serverSocket;
}

int acceptClient(int serverSocket) {
    // Accept the client.
    struct sockaddr_storage clientAddr;
    socklen_t clientAddrLen = sizeof clientAddr;
    int clientSocket = accept(serverSocket, (struct sockaddr *) &clientAddr, &clientAddrLen);
    if (clientSocket == -1) {
        perror("accept");
        return -1;
    }
    char clientIP[INET_ADDRSTRLEN];
    inet_ntop(clientAddr.ss_family, &((struct sockaddr_in *) &clientAddr)->sin_addr, clientIP, sizeof clientIP);
    printf("Client %s:%d connected!\n", clientIP, ((struct sockaddr_in *) &clientAddr)->sin_port);
    return clientSocket;
}

bool sendBoolean(int socket, bool boolean) {
    return send(socket, &boolean, sizeof(bool), 0);
}
ssize_t sendInt(int socket, int data) {
    return send(socket, &data, sizeof(int), 0);
}
ssize_t sendSize(int socket, size_t data) {
    return send(socket, &data, sizeof(size_t), 0);
}
ssize_t sendString(int socket, const char *data) {
    return send(socket, data, strlen(data), 0);
}
bool recvBoolean(int socket) {
    bool buffer;
    recv(socket, &buffer, 1, 0);
    return buffer;
}
int recvInt(int socket) {
    int buffer;
    recv(socket, &buffer, sizeof buffer, 0);
    return buffer;
}
ssize_t recvString(int socket, char *buffer, size_t size) {
    return recv(socket, buffer, size, 0);
}