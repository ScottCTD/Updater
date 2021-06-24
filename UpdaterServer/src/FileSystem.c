//
// Created by Scott on 2021/6/21.
//

#include "FileSystem.h"

char *generateLocalFileUniqueIdentifier(LocalFile *file) {
    char buffer[BUFFER_SIZE];
    sprintf(buffer, "%s%ld", file->name, file->size);
    char *result = malloc(strlen(buffer) + 1);
    strcpy(result, buffer);
    return result;
}

cJSON *localFileToJson(LocalFile *file)  {
    cJSON *jsonObject = cJSON_CreateObject();
    cJSON_AddStringToObject(jsonObject, "Path", file->path);
    cJSON_AddStringToObject(jsonObject, "Name", file->name);
    char *identifier = generateLocalFileUniqueIdentifier(file);
    cJSON_AddStringToObject(jsonObject, "Identifier", identifier);
    free(identifier);
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
    file->size = fileStat.st_size;
    return file;
}

void freeLocalFile(LocalFile *file) {
    free(file->path);
    if (file->path != file->name) {
        free(file->name);
    }
    free(file);
}
