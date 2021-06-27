#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/stat.h>
#include <stdbool.h>
#include <threads.h>
#include <dirent.h>

#include "cJSON.h"
#include "FileSystem.h"
#include "NetworkSystem.h"

#define PORT (30001)
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
    cJSON_AddArrayToObject(serverConfig, "Sync Directories");
    cJSON_AddArrayToObject(serverConfig, "Sync Files");
    cJSON_AddArrayToObject(serverConfig, "Whitelist");
    FILE *file = fopen("config.json", "wb");
    char *defaultConfigString = cJSON_Print(serverConfig);
    fwrite(defaultConfigString, 1, strlen(defaultConfigString), file);
    fclose(file);
    free(defaultConfigString);
}
void initConfig() {
    printf("Initializing config...\n");
    if (access("config.json", F_OK) != 0) {
        createConfig();
    }
    FILE *file = fopen("config.json", "rb");
    if (file == NULL) {
        printf("Failed to open config file Config.json!\n");
        return;
    }
    char buffer[BUFFER_SIZE];
    fread(buffer, 1, BUFFER_SIZE, file);
    fclose(file);
    serverConfig = cJSON_Parse(buffer);
    if (serverConfig == NULL) {
        createConfig();
    }
    // TODO Remove it, the return value of cJSON_Print needs to be freed.
    printf("%s\n", cJSON_Print(serverConfig));
    printf("Success!\n");
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
    printf("Initializing server data...\n");
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
    char path[BUFFER_SIZE] = "resources/";
    cJSON *temp = NULL;
    // Scan configured sync directories.
    cJSON_ArrayForEach(temp, cJSON_GetObjectItemCaseSensitive(serverConfig, "Sync Directories")) {
        char *rawPath = cJSON_GetStringValue(temp);
        memset(path + 10, 0, BUFFER_SIZE - 10);
        strcat(path, rawPath);
        LocalFile *file = newLocalFile(path);
        if (file == NULL) {
            printf("%s does not exist!\n", path);
            stop(-1);
        } else {
            scanDirectory(file, serverDirArrayJson, serverFileArrayJson);
        }
    }
    // Scan configured sync files.
    cJSON_ArrayForEach(temp, cJSON_GetObjectItemCaseSensitive(serverConfig, "Sync Files")) {
        char *rawPath = cJSON_GetStringValue(temp);
        memset(path + 10, 0, BUFFER_SIZE - 10);
        strcat(path, rawPath);
        LocalFile *file = newLocalFile(path);
        if (file == NULL) {
            printf("%s does not exist!\n", path);
            stop(-1);
        } else {
            cJSON_AddItemToArray(serverFileArrayJson, localFileToJson(file));
        }
    }
    // TODO Remove it
    printf("%s\n", cJSON_Print(serverJson));
    printf("Success!\n");
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
            printf("Success!\n");
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
    if ((serverSocket = initSocketOnServer(PORT)) < 0) {
        return serverSocket;
    }

    thrd_t inputThread;
    thrd_create(&inputThread, asynHandleInput, NULL);
    thrd_detach(inputThread);

    while (true) {
        int clientSocket = acceptClient(serverSocket);
        if (clientSocket == -1) {
            continue;
        }
        thrd_t thread;
        thrd_create(&thread, asynHandleClient, &clientSocket);
        thrd_detach(thread);
    }
}