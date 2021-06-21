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
#include <threads.h>
#include <dirent.h>

#include "cJSON.h"

#define PORT (30001)
#define BACKLOG (20)
#define BUFFER_SIZE (8192)
#define RESOURCE_FOLDER "resources"

#define SYNC_MODE_ALL (0)
#define SYNC_MODE_HARD (1)
#define SYNC_MODE_SOFT (2)

int serverSocket = 0;
int syncMode = SYNC_MODE_HARD;
cJSON *serverConfig = NULL;
cJSON *serverJson = NULL;

void createConfig() {
    serverConfig = cJSON_CreateObject();
    cJSON_AddStringToObject(serverConfig, "Mode", "HARD");
}
void initConfig() {
    if (access("config.json", F_OK) != 0) {
        FILE *file = fopen("config.json", "wb");
        createConfig();
        char *defaultConfigString = cJSON_Print(serverConfig);
        fwrite(defaultConfigString, 1, strlen(defaultConfigString), file);
        fclose(file);
    }
    FILE *file = fopen("config.json", "rb");
    if (file == NULL) {
        printf("Failed to read config file Config.json!");
        return;
    }
    char buffer[BUFFER_SIZE];
    fread(buffer, 1, BUFFER_SIZE, file);
    fclose(file);
    serverConfig = cJSON_Parse(buffer);
    cJSON *mode = cJSON_GetObjectItemCaseSensitive(serverConfig, "Mode");
    if (strcmp("ALL", mode->valuestring) == 0) {
        syncMode = SYNC_MODE_ALL;
    } else if (strcmp("HARD", mode->valuestring) == 0) {
        syncMode = SYNC_MODE_HARD;
    } else if (strcmp("SOFT", mode->valuestring) == 0) {
        syncMode = SYNC_MODE_SOFT;
    }
}

void generateUniqueFileIdentifier(const char *path, const char *fileName, cJSON *jsonObject) {
    FILE *file = fopen(path, "rb");
    if (file == NULL) {
        printf("Failed to open file %s\n", path);
        return;
    }
    fseek(file, 0, SEEK_END);
    char buffer[BUFFER_SIZE];
    // Name Size
    sprintf(buffer, "%s%ld", fileName, ftell(file));
    fclose(file);
    cJSON_AddStringToObject(jsonObject, "Identifier", buffer);
}
void scanResources(DIR *directory, const char *path, cJSON *jsonArray, cJSON *directoriesArray) {
    struct dirent *dirEntry;
    while ((dirEntry = readdir(directory)) != NULL) {
        if (strcmp(".", dirEntry->d_name) == 0 || strcmp("..", dirEntry->d_name) == 0) {
            continue;
        }
        char temp[BUFFER_SIZE];
        strcpy(temp, path);
        strcat(temp, "/");
        strcat(temp, dirEntry->d_name);
        if (dirEntry->d_type == DT_DIR) {
            cJSON_AddItemToArray(directoriesArray, cJSON_CreateString(temp + 10));
            DIR *subDir = opendir(temp);
            scanResources(subDir, temp, jsonArray, directoriesArray);
        } else if (dirEntry->d_type == DT_REG) {
            cJSON *jsonObject = cJSON_CreateObject();
            cJSON_AddStringToObject(jsonObject, "Path", temp);
            generateUniqueFileIdentifier(temp, dirEntry->d_name, jsonObject);
            cJSON_AddItemToArray(jsonArray, jsonObject);
        }
    }
}
void initResources() {
    printf("Processing Data...\n");
    serverJson = cJSON_CreateObject();
    cJSON *serverDirectories = cJSON_AddArrayToObject(serverJson, "Directories");
    int value;
    DIR *resourceDir = opendir(RESOURCE_FOLDER);
    if (resourceDir == NULL) {
        if ((value = mkdir(RESOURCE_FOLDER, S_IRWXU)) != 0) {
            printf("Failed to create resource folder, code: %d\n", value);
        }
    }
    cJSON *json = cJSON_AddArrayToObject(serverJson, "Resources");
    scanResources(resourceDir, RESOURCE_FOLDER, json, serverDirectories);

    char *temp = cJSON_Print(serverJson);
    printf("%s %lu\n", temp, strlen(temp));
}

int initSocket() {
    printf("Server is starting!\n");
    int value;
    // Get our host address info.
    struct addrinfo hints, *serverInfo;
    memset(&hints, 0, sizeof hints);
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_flags = AI_PASSIVE;
    char port[6];
    sprintf(port, "%d", htons(PORT));
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
    return 0;
}
int acceptClient() {
    // Accpet the client.
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
void stop() {
    cJSON_Delete(serverConfig);
    cJSON_Delete(serverJson);
    close(serverSocket);
    exit(0);
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

int asynHandleInput(void *arg) {
    while (true) {
        char buffer[BUFFER_SIZE];
        scanf("%s", buffer);
        if (strcmp("exit", buffer) == 0) {
            stop();
        }
        if (strcmp("update", buffer) == 0) {
            cJSON_Delete(serverJson);
            serverJson = cJSON_CreateObject();
            initResources(serverJson);
        }
    }
}

int asynHandleClient(void *arg) {
    int clientSocket = *(int *) arg;
    struct timespec sleepTime;
    sleepTime.tv_sec = 1;
    while (true) {
        thrd_sleep(&sleepTime, NULL);
        char buffer[BUFFER_SIZE];
        memset(buffer, 0, BUFFER_SIZE);
        recvString(clientSocket, buffer, BUFFER_SIZE);
        if (strcmp("config", buffer) == 0) {
            char *configString = cJSON_Print(serverConfig);
            sendSize(clientSocket, strlen(configString));
            sendString(clientSocket, configString);
            continue;
        }
        if (strcmp("list", buffer) == 0) {
            char *jsonString = cJSON_Print(serverJson);
            sendSize(clientSocket, strlen(jsonString));
            sendString(clientSocket, jsonString);
            continue;
        }
        if (strcmp("download", buffer) == 0) {

            continue;
        }
        if (strcmp("stop", buffer) == 0) {
            printf("Client %d disconnected!\n", clientSocket);
            close(clientSocket);
            thrd_exit(0);
        }
    }
}

int main() {
    initConfig();
    initResources();
    int result;
    if ((result = initSocket()) != 0) {
        return result;
    }

    thrd_t inputThread;
    thrd_create(&inputThread, asynHandleInput, NULL);
    thrd_detach(inputThread);

    while (true) {
        int clientSocket = acceptClient();
        if (clientSocket == -1) {
            continue;
        }
        thrd_t thread;
        thrd_create(&thread, asynHandleClient, &clientSocket);
        thrd_detach(thread);
    }
}