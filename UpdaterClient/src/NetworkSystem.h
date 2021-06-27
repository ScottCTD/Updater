//
// Created by Scott on 2021/6/25.
//

#ifndef UPDATERCLIENT_NETWORKSYSTEM_H
#define UPDATERCLIENT_NETWORKSYSTEM_H

#include <winsock2.h>
#include <stdio.h>
#include <windows.h>
#include <ws2tcpip.h>
#include <stdbool.h>
#include <time.h>

#ifndef BUFFER_SIZE
#define BUFFER_SIZE (8192)
#endif

SOCKET initSocketOnClient(const char *serverAddress, unsigned short serverPort);

int sendString(SOCKET socket, char *string);

bool recvBoolean(SOCKET socket);
void recvString(SOCKET socket, size_t totalSize, char *string);
size_t recvSize(SOCKET socket);

void downloadFileFromServer(SOCKET serverSocket, char *serverFilePath, char *clientFilePath);

#endif //UPDATERCLIENT_NETWORKSYSTEM_H
