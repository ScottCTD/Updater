//
// Created by Scott on 2021/6/21.
//

#include "FileSystem.h"

/**
 * ==================================================================================
 *                     LocalFile Functions Start From Here
 * ==================================================================================
 */

char *generateLocalFileUniqueIdentifier(LocalFile *file) {
    char buffer[BUFFER_SIZE];
    sprintf(buffer, "%s%zu", file->name, file->size);
    char *result = malloc(strlen(buffer) + 1);
    strcpy(result, buffer);
    return result;
}

cJSON *localFileToJson(LocalFile *file)  {
    cJSON *jsonObject = cJSON_CreateObject();
    cJSON_AddStringToObject(jsonObject, "Path", file->path);
    cJSON_AddStringToObject(jsonObject, "Name", file->name);
    cJSON_AddStringToObject(jsonObject, "Identifier", file->identifier);
    return jsonObject;
}

LocalFile *localFileFromJson(cJSON *jsonObj) {
    return newLocalFile(cJSON_GetStringValue(cJSON_GetObjectItemCaseSensitive(jsonObj, "Path")));
}

LocalFile *newLocalFile(const char *path) {
    size_t pathLength = strlen(path);
    LocalFile *file = malloc(sizeof(LocalFile));
    file->path = malloc(pathLength + 1);
    strcpy(file->path, path);
    char buffer[pathLength];
    strcpy(buffer, path);
    if (buffer[pathLength - 1] == '/') {
        buffer[pathLength - 1] = '\0';
    }
    char *name = strrchr(buffer, '/');
    if (name == NULL) {
        file->name = file->path;
    } else {
        file->name = malloc(strlen(name) + 1);
        ++name;
        strcpy(file->name, name);
    }
    struct stat fileStat;
    if (stat(path, &fileStat) != 0) {
        perror("stat");
        return NULL;
    }
    file->isDirectory = S_ISDIR(fileStat.st_mode);
    if (file->isDirectory) {
        file->size = 0;
        file->identifier = NULL;
    } else {
        file->size = fileStat.st_size;
        file->identifier = generateLocalFileUniqueIdentifier(file);
    }
    return file;
}

void freeLocalFile(LocalFile *file) {
    free(file->path);
    if (file->path != file->name) {
        free(file->name);
    }
    free(file->identifier);
    free(file);
}

int localFileComparator(const void *file01, const void *file02) {
    const LocalFile *lf01 = file01, *lf02 = file02;
    return strcmp(lf01->identifier, lf02->identifier);
}

/**
 * ==================================================================================
 *                             LocalFile Functions End
 * ==================================================================================
 */

/**
 * ==================================================================================
 *                      RemoteFile Functions Start From Here
 * ==================================================================================
 */

RemoteFile *remoteFileFromJson(cJSON *json) {
    RemoteFile *file = malloc(sizeof(RemoteFile));
    char *path = cJSON_GetStringValue(cJSON_GetObjectItemCaseSensitive(json, "Path"));
    char *name = cJSON_GetStringValue(cJSON_GetObjectItemCaseSensitive(json, "Name"));
    char *identifier = cJSON_GetStringValue(cJSON_GetObjectItemCaseSensitive(json, "Identifier"));
    file->path = malloc(strlen(path) + 1);
    file->name = malloc(strlen(name) + 1);
    file->identifier = malloc(strlen(identifier) + 1);
    strcpy(file->path, path);
    strcpy(file->name, name);
    strcpy(file->identifier, identifier);
    return file;
}

void freeRemoteFile(RemoteFile *file) {
    free(file->path);
    free(file->name);
    free(file->identifier);
    free(file);
}

int remoteFileComparator(const void *file01, const void *file02) {
    const RemoteFile *rf01 = file01, *rf02 = file02;
    return strcmp(rf01->identifier, rf02->identifier);
}

/**
 * ==================================================================================
 *                             RemoteFile Functions End
 * ==================================================================================
 */

/**
 * ==================================================================================
 *                            Utility Functions Start
 * ==================================================================================
 */

void mkdirs(const char *path) {
    if (access(path, F_OK) != 0) {
        if (mkdir(path) != 0) {
            char buffer[BUFFER_SIZE];
            memset(buffer, 0, BUFFER_SIZE);
            char *pos = strrchr(path, '/');
            if (pos != NULL) {
                strncpy(buffer, path, pos - path);
                mkdirs(buffer);
            }
            mkdir(path);
        }
    }
}

/**
 * ==================================================================================
 *                            Utility Functions End
 * ==================================================================================
 */

