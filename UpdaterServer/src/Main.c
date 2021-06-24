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
#include "FileSystem.h"

#define PORT (30001)
#define BACKLOG (20)
#define BUFFER_SIZE (8192)
#define RESOURCE_FOLDER "resources"

int serverSocket = 0;
cJSON *serverConfig = NULL;
cJSON *serverJson = NULL;

void stop(int status) {
    cJSON_Delete(serverConfig);
    cJSON_Delete(serverJson);
    close(serverSocket);
    exit(status);
}

void createConfig() {
    serverConfig = cJSON_CreateObject();
    cJSON *syncFilesArrayJson = cJSON_AddArrayToObject(serverConfig, "Sync Files / Directories");
    cJSON_AddItemToArray(syncFilesArrayJson, cJSON_CreateString("resources/exampleDir"));
    cJSON_AddArrayToObject(serverConfig, "Whitelist");
}
void initConfig() {
    printf("Initializing config!\n");
    if (access("config.json", F_OK) != 0) {
        FILE *file = fopen("config.json", "wb");
        createConfig();
        char *defaultConfigString = cJSON_Print(serverConfig);
        fwrite(defaultConfigString, 1, strlen(defaultConfigString), file);
        fclose(file);
    }
    FILE *file = fopen("config.json", "rb");
    if (file == NULL) {
        printf("Failed to read config file Config.json!\n");
        return;
    }
    char buffer[BUFFER_SIZE];
    fread(buffer, 1, BUFFER_SIZE, file);
    fclose(file);
    serverConfig = cJSON_Parse(buffer);
    printf("%s\n", cJSON_Print(serverConfig));
}

void scanDirectory(LocalFile *directory, cJSON *directoriesArray, cJSON *filesArray) {
    DIR *dir = opendir(directory->path);
    struct dirent *entry = NULL;
    while ((entry = readdir(dir)) != NULL) {
        if (strcmp(".", entry->d_name) == 0 ||
            strcmp("..", entry->d_name) == 0) {
            continue;
        }
        char path[BUFFER_SIZE];
        memset(path, 0, BUFFER_SIZE);
        strcat(path, directory->path);
        strcat(path, "/");
        strcat(path, entry->d_name);
        LocalFile *file = newLocalFile(path);
        if (file->isDirectory) {
            cJSON_AddItemToArray(directoriesArray, cJSON_CreateString(file->path));
            scanDirectory(file, directoriesArray, filesArray);
        } else {
            cJSON_AddItemToArray(filesArray, localFileToJson(file));
        }
        freeLocalFile(file);
    }
    closedir(dir);
}
void initData() {
    printf("Initializing server data!\n");
    serverJson = cJSON_CreateObject();
    cJSON *serverDirArrayJson = cJSON_AddArrayToObject(serverJson, "Directories");
    cJSON *serverFileArrayJson = cJSON_AddArrayToObject(serverJson, "Files");
    // Create resources folder.
    int value;
    if (access(RESOURCE_FOLDER, F_OK) != 0) {
        if ((value = mkdir(RESOURCE_FOLDER, S_IRWXU)) != 0) {
            printf("Failed to create resource folder, code: %d\n", value);
            exit(-1);
        }
    }
    cJSON *temp = NULL;
    cJSON_ArrayForEach(temp, cJSON_GetObjectItemCaseSensitive(serverConfig, "Sync Files / Directories")) {
        char *path = cJSON_GetStringValue(temp);
        LocalFile *file = newLocalFile(path);
        if (file == NULL) {
            printf("%s does not exist!\n", path);
            stop(-1);
        } else {
            if (file->isDirectory) {
                scanDirectory(file, serverDirArrayJson, serverFileArrayJson);
            } else {
                cJSON_AddItemToArray(serverFileArrayJson, localFileToJson(file));
            }
        }
    }
    printf("%s\n", cJSON_Print(serverJson));
}

int initSocket() {
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

int asynHandleInput(void *arg) {
    while (true) {
        char buffer[BUFFER_SIZE];
        scanf("%s", buffer);
        if (strcmp("exit", buffer) == 0) {
            stop(0);
        }
        if (strcmp("update", buffer) == 0) {
            initConfig();
            initData();
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
            sendBoolean(clientSocket, true);
            memset(buffer, 0, BUFFER_SIZE);
            recvString(clientSocket, buffer, BUFFER_SIZE);
            LocalFile *file = newLocalFile(buffer);
            sendSize(clientSocket, file->size);
            FILE *f = fopen(buffer, "rb");
            size_t bytes;
            memset(buffer, 0, BUFFER_SIZE);
            while ((bytes = fread(buffer, 1, BUFFER_SIZE, f)) == BUFFER_SIZE) {
                send(clientSocket, buffer, bytes, 0);
            }
            if (bytes != 0) {
                send(clientSocket, buffer, bytes, 0);
            }
            fclose(f);
            freeLocalFile(file);
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
    printf("Starting Server...\n");
    initConfig();
    initData();
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