//
// Created by Scott on 2021/6/25.
//

#include "NetworkSystem.h"

int sendString(SOCKET socket, char *string) {
    return send(socket, string, (int) strlen(string), 0);
}

bool recvBoolean(SOCKET socket) {
    bool boolean;
    recv(socket, (char *) &boolean, 1, 0);
    return boolean;
}
void recvString(SOCKET socket, size_t totalSize, char *string) {
    char buffer[BUFFER_SIZE];
    size_t bytes, total = 0;
    while (total < totalSize && (bytes = recv(socket, buffer, BUFFER_SIZE, 0)) > 0) {
        strncat(string, buffer, bytes);
        total += bytes;
    }
}
size_t recvSize(SOCKET socket) {
    size_t size = 0;
    recv(socket, (char *) &size, sizeof(size_t), 0);
    return size;
}

void downloadFileFromServer(SOCKET serverSocket, char *serverFilePath, char *clientFilePath) {
    sendString(serverSocket, "download");
    if (recvBoolean(serverSocket)) {
        sendString(serverSocket, serverFilePath);
        char buffer[BUFFER_SIZE];
        size_t size = recvSize(serverSocket);
        size_t bytes, total = 0;
        FILE *file = fopen(clientFilePath, "wb");
        while (total < size && (bytes = recv(serverSocket, buffer, BUFFER_SIZE, 0)) > 0) {
            fwrite(buffer, 1, bytes, file);
            total += bytes;
        }
    }
}

SOCKET initSocketOnClient(const char *serverAddress, unsigned short serverPort) {
    WSADATA wsaData;
    SOCKET serverSocket;
    int result;
    if ((result = WSAStartup(MAKEWORD(2, 2), &wsaData)) != 0) {
        printf("WSAStartup Failed with code %d\n", result);
        return -1;
    }

    struct addrinfo hint, *serverInfo;
    memset(&hint, 0, sizeof(struct addrinfo));
    hint.ai_family = AF_INET;
    hint.ai_socktype = SOCK_STREAM;
    char port[6];
    sprintf(port, "%d", htons(serverPort));
    if ((result = getaddrinfo(serverAddress, port, &hint, &serverInfo)) != 0) {
        printf("getaddrinfo Failed with code %d\n", result);
        WSACleanup();
        return -1;
    }

    struct sockaddr_in *serverAddressIn = ((struct sockaddr_in *) serverInfo->ai_addr);
    printf("Connecting to server %s:%d...\n", inet_ntoa(serverAddressIn->sin_addr), serverAddressIn->sin_port);

    struct addrinfo *realAddrInfo;
    for (realAddrInfo = serverInfo; realAddrInfo != NULL; realAddrInfo = realAddrInfo->ai_next) {
        if ((serverSocket = socket(realAddrInfo->ai_family, realAddrInfo->ai_socktype, realAddrInfo->ai_protocol)) == INVALID_SOCKET) {
            printf("socket Failed with code %d\n", WSAGetLastError());
            continue;
        }
        if ((result = connect(serverSocket, realAddrInfo->ai_addr, (int) realAddrInfo->ai_addrlen)) == SOCKET_ERROR) {
            printf("connect Failed with code %d\n", result);
            continue;
        }
        break;
    }
    if (realAddrInfo == NULL || serverSocket == INVALID_SOCKET) {
        printf("Failed to connect to the server!\n");
        WSACleanup();
        return -1;
    }

    serverAddressIn = ((struct sockaddr_in *) realAddrInfo->ai_addr);
    printf("Connected to %s:%d!\n", inet_ntoa(serverAddressIn->sin_addr), serverAddressIn->sin_port);
    freeaddrinfo(serverInfo);
    return serverSocket;
}

