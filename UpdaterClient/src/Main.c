#include <stdio.h>
#include <winsock2.h>
#include <windows.h>
#include <ws2tcpip.h>
#include <stdbool.h>

#include "cJSON.h"
#include "dirent.h"

#define SERVER_IP "127.0.0.1"
#define PORT (30001)
#define BUFFER_SIZE (8192)

#define SYNC_MODE_ALL (0)
#define SYNC_MODE_HARD (1)
#define SYNC_MODE_SOFT (2)

int syncMode = SYNC_MODE_HARD;
SOCKET serverSocket = INVALID_SOCKET;
cJSON *clientJson = NULL;

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
void scanResources(DIR *directory, const char *path, cJSON *jsonArray) {
    static char buffer[256];
    char *executableName;
    if (buffer[0] == 0) {
        GetModuleFileName(NULL, buffer, 256);
        executableName = strrchr(buffer, '\\') + 1;
    }
    struct dirent *dirEntry;
    while ((dirEntry = readdir(directory)) != NULL) {
        if (strcmp(".", dirEntry->d_name) == 0 ||
            strcmp("..", dirEntry->d_name) == 0 ||
            strcmp(executableName, dirEntry->d_name) == 0) {
            continue;
        }
        char temp[BUFFER_SIZE];
        strcpy(temp, path);
        strcat(temp, "/");
        strcat(temp, dirEntry->d_name);
        if (dirEntry->d_type == DT_DIR) {
            DIR *subDir = opendir(temp);
            scanResources(subDir, temp, jsonArray);
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
    clientJson = cJSON_CreateObject();
    int value;
    DIR *resourceDir = opendir(".");
    if (resourceDir == NULL) {
        printf("Failed to create resource folder, code: %d\n", value);
        exit(-1);
    }
    cJSON *json = cJSON_AddArrayToObject(clientJson, "Resources");
    scanResources(resourceDir, ".", json);
    printf("%s\n", cJSON_Print(clientJson));
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

void stop() {
    sendString("stop");
    cJSON_Delete(clientJson);
    closesocket(serverSocket);
    WSACleanup();
}

bool searchInJsonArray(cJSON *fileIdentifier, cJSON *fileList) {
    cJSON *temp = NULL;
    cJSON_ArrayForEach(temp, fileList) {
        cJSON *identifier = cJSON_GetObjectItemCaseSensitive(temp, "Identifier");
        if (cJSON_IsString(identifier)) {
            if (strcmp(identifier->valuestring, fileIdentifier->valuestring) == 0) {
                return true;
            }
        }
    }
    return false;
}

int main() {
    int result;
    initResources();
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
    printf("%d\n", syncMode);

    sendString("list");
    printf("Listing server files!\n");
    size = recvSize();
    char *serverJsonString = recvString(size);
    printf("%s\n", serverJsonString);

    printf("Processing Data!\n");
    cJSON *serverJson = cJSON_Parse(serverJsonString);
    cJSON *serverResources = cJSON_GetObjectItemCaseSensitive(serverJson, "Resources");
    cJSON *clientResources = cJSON_GetObjectItemCaseSensitive(clientJson, "Resources");
    cJSON *serverFile;
    cJSON_ArrayForEach(serverFile, serverResources) {
        cJSON *identifier = cJSON_GetObjectItemCaseSensitive(serverFile, "Identifier");
        if (!searchInJsonArray(identifier, clientResources)) {
            printf("%s\n", identifier->valuestring);
        }
    }

    free(serverJsonString);
    stop();
    return 0;
}

