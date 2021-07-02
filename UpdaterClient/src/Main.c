#include <stdio.h>
#include <winsock2.h>
#include <windows.h>

#include "cJSON.h"
#include "FileSystem.h"
#include "NetworkSystem.h"

#define SERVER_IP "127.0.0.1"
#define PORT (30001)
#define BUFFER_SIZE (8192)

char *executableName = NULL;
SOCKET serverSocket = INVALID_SOCKET;

int comparatorLocalFileIdentifier(const LocalFile *a, const void *b) {
    if (a->identifier == NULL) {
        return -1;
    }
    const char *c = b;
    return strcmp(a->identifier, c);
}

void deleteExtraLocalFiles(LocalFile *localFileHead, cJSON *serverSyncDirJsonArray, cJSON *serverDirJsonArray, cJSON *serverFileJsonArray) {
    for (LocalFile *localFile = localFileHead; localFile != NULL; localFile = localFile->next) {
        if (localFile->child != NULL) {
            deleteExtraLocalFiles(localFile->child, serverSyncDirJsonArray, serverDirJsonArray, serverFileJsonArray);
        }
        bool found = false;
        cJSON *tempJson = NULL;
        if (localFile->isDirectory) {
            cJSON_ArrayForEach(tempJson, serverDirJsonArray) {
                char *serverDirPath = cJSON_GetStringValue(tempJson);
                if (strcmp(localFile->path, serverDirPath + 10) == 0) {
                    found = true;
                    break;
                }
            }
            cJSON_ArrayForEach(tempJson, serverSyncDirJsonArray) {
                char *serverDirPath = cJSON_GetStringValue(tempJson);
                if (strcmp(localFile->path, serverDirPath) == 0) {
                    found = true;
                    break;
                }
            }
            if (!found) {
                deleteDirectory(localFile->path);
            }
        } else {
            cJSON_ArrayForEach(tempJson, serverFileJsonArray) {
                char *serverFileIdentifier = cJSON_GetStringValue(cJSON_GetObjectItemCaseSensitive(tempJson, "Identifier"));
                if (strcmp(localFile->identifier, serverFileIdentifier) == 0) {
                    found = true;
                    break;
                }
            }
            if (!found) {
                unlink(localFile->path);
            }
        }
    }
}

void stop(int status) {
    sendString(serverSocket, "stop");
    free(executableName);
    closesocket(serverSocket);
    WSACleanup();
    exit(status);
}

int main() {
    char buffer[BUFFER_SIZE];
    memset(buffer, 0, BUFFER_SIZE);
    // Get our executable name
    GetModuleFileName(NULL, buffer, BUFFER_SIZE);
    char *pos = strrchr(buffer, '\\') + 1;
    executableName = malloc(strlen(pos) + 1);
    strcpy(executableName, pos);

    if ((serverSocket = initSocketOnClient(SERVER_IP, PORT)) == -1) {
        return (int) serverSocket;
    }

    // Retrieve Config from server
    sendString(serverSocket, "config");
    printf("Retrieving server config...\n");
    size_t size = recvSize(serverSocket);
    char serverConfigString[size + 1];
    memset(serverConfigString, 0, size + 1);
    recvString(serverSocket, size, serverConfigString);
    cJSON *serverConfigJson = cJSON_Parse(serverConfigString);
    if (serverConfigJson == NULL) {
        printf("Failed to retrieve complete server config! Please retry later.\n");
        stop(-1);
    }
    printf("Success!\n");

    // List all files in server
    sendString(serverSocket, "list");
    printf("Listing server files!\n");
    size = recvSize(serverSocket);
    char serverDataStringRaw[size + 1];
    memset(serverDataStringRaw, 0, size + 1);
    recvString(serverSocket, size, serverDataStringRaw);
    char *serverDataString = UTF8ToGBK(serverDataStringRaw);
    cJSON *serverDataJson = cJSON_Parse(serverDataString);
    printf("%s\n", serverDataString);
    free(serverDataString);
    if (serverDataJson == NULL) {
        printf("Failed to list server files! Please retry later.\n");
        stop(-1);
    }
    // TODO Remove it
    printf("%s\n", cJSON_Print(serverDataJson));
    printf("Success!\n");

    // Create missing dirs
    cJSON *serverTempJson = NULL;
    printf("Creating missing directories...\n");
    cJSON *serverSyncDirJsonArray = cJSON_GetObjectItemCaseSensitive(serverConfigJson, "Sync Directories");
    cJSON_ArrayForEach(serverTempJson, serverSyncDirJsonArray) {
        mkdirs(cJSON_GetStringValue(serverTempJson));
    }
    cJSON *serverDirJsonArray = cJSON_GetObjectItemCaseSensitive(serverDataJson, "Directories");
    cJSON_ArrayForEach(serverTempJson, serverDirJsonArray) {
        mkdirs(cJSON_GetStringValue(serverTempJson) + 10);
    }
    printf("Success!\n");

    // Scan local files based on sync directories and sync files.
    printf("Scanning local files and directories...\n");
    LocalFile *clientDataHead = NULL;
    cJSON_ArrayForEach(serverTempJson, serverSyncDirJsonArray) {
        char *synDirPath = cJSON_GetStringValue(serverTempJson);
        LocalFile *dir = newLocalFile(synDirPath);
        if (clientDataHead == NULL) {
            clientDataHead = dir;
        } else {
            LocalFile *temp = NULL;
            for (temp = clientDataHead; temp->next != NULL; temp = temp->next);
            temp->next = dir;
            dir->prev = temp;
        }
        createLocalFileTree(dir);
    }
    printf("Success!\n");

    // Download missing files.
    printf("Downloading missing files...\n");
    printf("");
    cJSON *serverFileJsonArray = cJSON_GetObjectItemCaseSensitive(serverDataJson, "Files");
    cJSON_ArrayForEach(serverTempJson, serverFileJsonArray) {
        char *serverFileIdentifier = cJSON_GetStringValue(cJSON_GetObjectItemCaseSensitive(serverTempJson, "Identifier"));
        LocalFile *result = searchLocalFileInFileTreeWithComparator(clientDataHead, serverFileIdentifier, comparatorLocalFileIdentifier);
        if (result == NULL) {
            char *serverFilePathGBK = cJSON_GetStringValue(cJSON_GetObjectItemCaseSensitive(serverTempJson, "Path"));
            char *serverFilePath = GBKToUTF8(serverFilePathGBK);
            printf("\rDownloading %s\n", serverFilePathGBK);
            downloadFileFromServer(serverSocket, serverFilePath, serverFilePathGBK + 10);
            free(serverFilePath);
        }
    }
    printf("\r");
    printf("Success!\n");

    // Delete other files and directories.
    printf("Deleting extra files and directories...\n");
    deleteExtraLocalFiles(clientDataHead, serverSyncDirJsonArray, serverDirJsonArray, serverFileJsonArray);
    printf("Success!\n");

    cJSON_Delete(serverConfigJson);
    cJSON_Delete(serverDataJson);
    freeLocalFile(clientDataHead);
    stop(0);
}

