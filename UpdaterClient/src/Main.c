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
#define PORT (30001)
#define BUFFER_SIZE (8192)

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

void scanDirectory(LocalFile *directory, ArrayList *dirList, ArrayList *fileList) {
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
        LocalFile *file = newLocalFile(path);
        if (file->isDirectory) {
            arrayListAdd(dirList, &file->path);
            scanDirectory(file, dirList, fileList);
        } else {
            arrayListAdd(fileList, file);
        }
    }
    closedir(dir);
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

void stop(int status) {
    sendString("stop");
    cJSON_Delete(clientJson);
    closesocket(serverSocket);
    WSACleanup();
    exit(status);
}

int remoteLocalFileComparator(const void *remoteFile, const void *localFile) {
    return strcmp(((const LocalFile *) localFile)->identifier, ((const RemoteFile *)remoteFile)->identifier);
}

int localRemoteFileComparator(const void *localFile, const void *remoteFile) {
    return strcmp(((const LocalFile *) localFile)->identifier, ((const RemoteFile *)remoteFile)->identifier);
}

int remoteLocalDirPathComparator(const void *remotePath, const void *localPath) {
    char *rp = *((char **) remotePath);
    char *lp = *((char **) localPath);
    return strcmp(rp, lp);
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
    cJSON *serverConfig = cJSON_Parse(serverConfigString);
    cJSON *serverTempJson = NULL;
    // Create missing root dirs and record client dirs and files.
    ArrayList *clientDirList = arrayListNew(sizeof(char *));
    ArrayList *clientFileList = arrayListNew(sizeof(LocalFile));
    cJSON_ArrayForEach(serverTempJson, cJSON_GetObjectItemCaseSensitive(serverConfig, "Sync Files / Directories")) {
        char *path = cJSON_GetStringValue(serverTempJson) + 10;
        mkdirs(path);
        LocalFile *dir = newLocalFile(path);
        scanDirectory(dir, clientDirList, clientFileList);
        freeLocalFile(dir);
    }

    sendString("list");
    printf("Listing server files!\n");
    size = recvSize();
    char *serverJsonString = recvString(size);
    printf("%s\n", serverJsonString);

    printf("Synchronizing files...\n");
    cJSON *serverJson = cJSON_Parse(serverJsonString);
    cJSON *serverDirArrayJson = cJSON_GetObjectItemCaseSensitive(serverJson, "Directories");
    // Create missing dirs and collect server dirs into an ArrayList
    ArrayList *serverDirList = arrayListNew(sizeof(char *));
    cJSON_ArrayForEach(serverTempJson, serverDirArrayJson) {
        char *path = serverTempJson->valuestring + 10;
        mkdirs(path);
        arrayListAdd(serverDirList, &path);
    }
    // Collect server files into an ArrayList
    ArrayList *serverFileList = arrayListNew(sizeof(RemoteFile));
    cJSON_ArrayForEach(serverTempJson, cJSON_GetObjectItemCaseSensitive(serverJson, "Files")) {
        RemoteFile *file = remoteFileFromJson(serverTempJson);
        arrayListAdd(serverFileList, file);
    }
    // Download missing files
    arrayListSort(clientFileList, localFileComparator);
    arrayListSort(serverFileList, remoteFileComparator);
    for (int i = 0; i < arrayListSize(serverFileList); ++i) {
        RemoteFile *remoteFile = arrayListGet(serverFileList, i);
        LocalFile *localFile = arrayListBinarySearch(clientFileList, remoteFile, remoteLocalFileComparator);
        if (localFile == NULL) {
            recvFile(remoteFile->path);
        }
    }
    // Delete other dirs
    arrayListSort(clientDirList, remoteLocalDirPathComparator);
    arrayListSort(serverDirList, remoteLocalDirPathComparator);
    for (int i = 0; i < arrayListSize(clientDirList); ++i) {
        char **clientDir = arrayListGet(clientDirList, i);
        char **remoteDir = arrayListBinarySearch(serverDirList, clientDir, remoteLocalDirPathComparator);
        if (remoteDir == NULL) {
            deleteDirectory(*clientDir);
        }
    }

    // Delete other files
    for (int i = 0; i < arrayListSize(clientFileList); ++i) {
        LocalFile *localFile = arrayListGet(clientFileList, i);
        RemoteFile *remoteFile = arrayListBinarySearch(serverFileList, localFile, localRemoteFileComparator);
        if (remoteFile == NULL) {
            unlink(localFile->path);
        }
    }

    // Deallocate memory
    free(serverConfigString);
    serverConfigString = NULL;
    free(serverJsonString);
    serverJsonString = NULL;
    for (int i = 0; i < arrayListSize(serverFileList); ++i) {
        RemoteFile *file = arrayListGet(serverFileList, i);
        free(file->name);
        free(file->path);
        free(file->identifier);
    }
    arrayListDelete(serverFileList);
    serverFileList = NULL;
    arrayListDelete(serverDirList);
    serverDirList = NULL;
    for (int i = 0; i < arrayListSize(clientFileList); ++i) {
        LocalFile *file = arrayListGet(clientFileList, i);
        free(file->name);
        free(file->path);
        free(file->identifier);
    }
    arrayListDelete(clientFileList);
    clientFileList = NULL;
    arrayListDelete(clientDirList);
    clientDirList = NULL;
    cJSON_Delete(serverJson);
    serverJson = NULL;
    free(serverConfig);
    serverConfig = NULL;
    stop(0);
}

