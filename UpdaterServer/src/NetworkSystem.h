//
// Created by Scott on 2021/6/26.
//

#ifndef UPDATERSERVER_NETWORKSYSTEM_H
#define UPDATERSERVER_NETWORKSYSTEM_H

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/stat.h>
#include <netinet/in.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <stdbool.h>

#define BACKLOG (20)

int initSocketOnServer(unsigned short serverPort);

int acceptClient(int serverSocket);

bool sendBoolean(int socket, bool boolean);
ssize_t sendInt(int socket, int data);
ssize_t sendSize(int socket, size_t data);
ssize_t sendString(int socket, const char *data);

bool recvBoolean(int socket);
int recvInt(int socket);
ssize_t recvString(int socket, char *buffer, size_t size);


#endif //UPDATERSERVER_NETWORKSYSTEM_H
