#include <stdio.h>
#include <winsock2.h>
#include <windows.h>
#include <ws2tcpip.h>
#include <stdbool.h>

#include "cJSON.h"
#include "dirent.h"
#include "ArrayList.h"
#include "FileSystem.h"

#define SERVER_IP "127.0.0.1"
#define PORT (30000)
#define BUFFER_SIZE (8192)

#define SYNC_MODE_ALL (0)
#define SYNC_MODE_HARD (1)
#define SYNC_MODE_SOFT (2)

int syncMode = SYNC_MODE_HARD;
char *executableName = NULL;
SOCKET serverSocket = INVALID_SOCKET;
cJSON *clientJson = NULL;

void deleteDirectory(char *pathToDir) {
    DIR *dir = opendir(pathToDir);
    struct dirent *entry = NULL;
    while ((entry = readdir(dir)) != NULL) {
        if (strcmp(".", entry->d_name) == 0 ||
            strcmp("..", entry->d_name) == 0 ||
            strcmp(executableName, entry->d_name) == 0) {
            continue;
        }
        char path[BUFFER_SIZE];
        strcat(path, pathToDir);
        strcat(path, "/");
        strcat(path, entry->d_name);
        if (entry->d_type == DT_DIR) {
            deleteDirectory(path);
        } else if (entry->d_type == DT_REG) {
            unlink(path);
        }
    }
    rmdir(pathToDir);
}

void scanDirectory(File *directory, cJSON *clientDirArrayJson, cJSON *clientFileArrayJson) {
    DIR *dir = opendir(directory->path);
    struct dirent *entry = NULL;
    while ((entry = readdir(dir)) != NULL) {
        if (strcmp(".", entry->d_name) == 0 ||
            strcmp("..", entry->d_name) == 0 ||
            strcmp(executableName, entry->d_name) == 0) {
            continue;
        }
        char path[BUFFER_SIZE];
        memset(path, 0, 256);
        strcat(path, directory->path);
        strcat(path, "/");
        strcat(path, entry->d_name);
        File *file = newFile(path);
        if (file->isDirectory) {
            cJSON_AddItemToArray(clientDirArrayJson, cJSON_CreateString(file->path));
            scanDirectory(file, clientDirArrayJson, clientFileArrayJson);
        } else {
            cJSON_AddItemToArray(clientFileArrayJson, fileToJson(file));
        }
        freeFile(file);
    }
}

int initSocket() {
    WSADATA wsaData;
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
    sprintf(port, "%d", htons(PORT));
    if ((result = getaddrinfo(SERVER_IP, port, &hint, &serverInfo)) != 0) {
        printf("getaddrinfo Failed with code %d\n", result);
        WSACleanup();
        return -1;
    }

    struct sockaddr_in *serverAddress = ((struct sockaddr_in *) serverInfo->ai_addr);
    printf("Connecting to server %s:%d\n", inet_ntoa(serverAddress->sin_addr), serverAddress->sin_port);

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

    serverAddress = ((struct sockaddr_in *) realAddrInfo->ai_addr);
    printf("Connected to %s:%d\n", inet_ntoa(serverAddress->sin_addr), serverAddress->sin_port);
    freeaddrinfo(serverInfo);
    return 0;
}

int sendString(char *string) {
    return send(serverSocket, string, (int) strlen(string), 0);
}

bool recvBoolean() {
    bool boolean;
    recv(serverSocket, (char *) &boolean, 1, 0);
    return boolean;
}
char *recvString(size_t totalSize) {
    char buffer[BUFFER_SIZE];
    size_t bytes, total = 0;
    char *string = NULL;
    while (total < totalSize && (bytes = recv(serverSocket, buffer, BUFFER_SIZE, 0)) > 0) {
        if (string == NULL) {
            string = calloc(bytes + 1, 1);
        } else {
            char *temp = realloc(string, strlen(string) + bytes + 1);
            string = temp;
        }
        strncat(string, buffer, bytes);
        total += bytes;
    }
    return string;
}
size_t recvSize() {
    size_t size = 0;
    recv(serverSocket, (char *) &size, sizeof(size_t), 0);
    return size;
}
void recvFile(char *serverFilePath) {
    sendString("download");
    if (recvBoolean()) {
        sendString(serverFilePath);
        char buffer[BUFFER_SIZE];
        size_t size = recvSize();
        size_t bytes, total = 0;
        FILE *file = fopen(serverFilePath + 10, "wb");
        while (total < size && (bytes = recv(serverSocket, buffer, BUFFER_SIZE, 0)) > 0) {
            fwrite(buffer, 1, bytes, file);
            total += bytes;
        }
    }
}

void stop() {
    sendString("stop");
    cJSON_Delete(clientJson);
    closesocket(serverSocket);
    WSACleanup();
}

int main() {
    char buffer[256];
    GetModuleFileName(NULL, buffer, 256);
    executableName = strrchr(buffer, '\\') + 1;

    int result;
    if ((result = initSocket()) != 0) {
        return result;
    }

    sendString("config");
    printf("Retrieving server config!\n");
    size_t size = recvSize();
    char *serverConfigString = recvString(size);
    cJSON *config = cJSON_Parse(serverConfigString);
    cJSON *mode = cJSON_GetObjectItemCaseSensitive(config, "Mode");
    if (strcmp("ALL", mode->valuestring) == 0) {
        syncMode = SYNC_MODE_ALL;
    } else if (strcmp("HARD", mode->valuestring) == 0) {
        syncMode = SYNC_MODE_HARD;
    } else if (strcmp("SOFT", mode->valuestring) == 0) {
        syncMode = SYNC_MODE_SOFT;
    }
    printf("Sync Mode = %d\n", syncMode);

    sendString("list");
    printf("Listing server files!\n");
    size = recvSize();
    char *serverJsonString = recvString(size);
    printf("%s\n", serverJsonString);

    printf("Processing Data\n");
    cJSON *serverJson = cJSON_Parse(serverJsonString);
    cJSON *serverDirJson = cJSON_GetObjectItemCaseSensitive(serverJson, "Directories");
    cJSON *serverRootDirJson = cJSON_GetObjectItemCaseSensitive(serverJson, "Root Directories");
    cJSON *serverTempJson = NULL;
    // Create missing dirs
    cJSON_ArrayForEach(serverTempJson, serverRootDirJson) {
        char *path = serverTempJson->valuestring + 10;
        if (opendir(path) == NULL) {
            mkdir(path);
        }
    }
    cJSON_ArrayForEach(serverTempJson, serverDirJson) {
        char *path = serverTempJson->valuestring + 10;
        if (opendir(path) == NULL) {
            mkdir(path);
        }
    }
    // Scan client files based on different sync modes.
    clientJson = cJSON_CreateObject();
    cJSON *clientDirArrayJson = cJSON_AddArrayToObject(clientJson, "Directories");
    cJSON *clientFileArrayJson = cJSON_AddArrayToObject(clientJson, "Files");
    if (syncMode == SYNC_MODE_ALL) {
        File *file = newFile(".");
        scanDirectory(file, clientDirArrayJson, clientFileArrayJson);
        freeFile(file);
    } else {
        cJSON_ArrayForEach(serverTempJson, serverRootDirJson) {
            File *file = newFile(serverTempJson->valuestring + 10);
            scanDirectory(file, clientDirArrayJson, clientFileArrayJson);
            freeFile(file);
        }
    }
    printf("%s\n", cJSON_Print(clientJson));
    // Download missing files
    cJSON *clientTempJson = NULL;
    cJSON *serverfilesJson = cJSON_GetObjectItemCaseSensitive(serverJson, "Files");
    cJSON_ArrayForEach(serverTempJson, serverfilesJson) {
        char *serverIdentifier = cJSON_GetStringValue(cJSON_GetObjectItemCaseSensitive(serverTempJson, "Identifier"));
        bool isClientHasFile = false;
        cJSON_ArrayForEach(clientTempJson, clientFileArrayJson) {
            char *clientIdentifier = cJSON_GetStringValue(cJSON_GetObjectItemCaseSensitive(clientTempJson, "Identifier"));
            if (strcmp(serverIdentifier, clientIdentifier) == 0) {
                isClientHasFile = true;
                break;
            }
        }
        if (!isClientHasFile) {
            char *serverFilePath = cJSON_GetStringValue(cJSON_GetObjectItemCaseSensitive(serverTempJson, "Path"));
            recvFile(serverFilePath);
        }
    }
    printf("%s\n", cJSON_Print(clientJson));
    // Delete other dirs
    cJSON_ArrayForEach(clientTempJson, clientDirArrayJson) {
        char *clientDirPath = cJSON_GetStringValue(clientTempJson);
        bool isServerHasDir = false;
        cJSON_ArrayForEach(serverTempJson, serverDirJson) {
            char *serverDirPath = cJSON_GetStringValue(serverTempJson) + 10;
            if (strcmp(clientDirPath, serverDirPath) == 0) {
                isServerHasDir = true;
                continue;
            }
        }
        if (!isServerHasDir) {
            deleteDirectory(clientDirPath);
        }
    }
    // Delete other files
    cJSON_ArrayForEach(clientTempJson, clientFileArrayJson) {
        char *clientFilePath = cJSON_GetStringValue(cJSON_GetObjectItemCaseSensitive(clientTempJson, "Path"));
        bool isServerHasFile = false;
        cJSON_ArrayForEach(serverTempJson, serverfilesJson) {
            char *serverFilePath = cJSON_GetStringValue(cJSON_GetObjectItemCaseSensitive(serverTempJson, "Path")) + 10;
            if (strcmp(clientFilePath, serverFilePath) == 0) {
                isServerHasFile = true;
                continue;
            }
        }
        if (!isServerHasFile) {
            unlink(clientFilePath);
        }
    }

    // Deallocate memory
    free(serverConfigString);
    free(serverJsonString);
    cJSON_Delete(serverJson);
    stop();
    return 0;
}

