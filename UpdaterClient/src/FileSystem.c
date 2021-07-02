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
    file->next = NULL;
    file->prev = NULL;
    file->child = NULL;
    return file;
}

LocalFile *searchLocalFileInFileTreeWithComparator(LocalFile *fileTreeStart, const void *element,
                                                   int (*comparator)(const LocalFile *, const void *)) {
    for (LocalFile *temp = fileTreeStart; temp != NULL; temp = temp->next) {
        if (comparator(temp, element) == 0) {
            return temp;
        }
        if (temp->child != NULL) {
            LocalFile *result = searchLocalFileInFileTreeWithComparator(temp->child, element, comparator);
            if (result != NULL) {
                return result;
            }
        }
    }
    return NULL;
}

void createLocalFileTree(LocalFile *startDir) {
    if (!startDir->isDirectory) {
        return;
    }
    char path[BUFFER_SIZE];
    strcpy(path, startDir->path);
    strcat(path, "\\*");
    WIN32_FIND_DATA entry;
    HANDLE handle;
    handle = FindFirstFile(path, &entry);
    do {
        if (strcmp(".", entry.cFileName) == 0 ||
            strcmp("..", entry.cFileName) == 0) {
            continue;
        }
        memset(path, 0, BUFFER_SIZE);
        strcat(path, startDir->path);
        strcat(path, "/");
        strcat(path, entry.cFileName);
        LocalFile *file = newLocalFile(path);
        if (startDir->child == NULL) {
            startDir->child = file;
        } else {
            LocalFile *temp;
            for (temp = startDir->child; temp->next != NULL; temp = temp->next);
            temp->next = file;
            file->prev = temp;
        }
        if (file->isDirectory) {
            createLocalFileTree(file);
        }
    } while (FindNextFile(handle, &entry));
}

void freeLocalFile(LocalFile *file) {
    for (LocalFile *temp = file; temp != NULL;) {
        if (temp->child != NULL) {
            freeLocalFile(temp->child);
        }
        free(temp->path);
        if (temp->path != temp->name) {
            free(temp->name);
        }
        free(temp->identifier);
        temp = temp->next;
        if (temp != NULL) {
            free(temp->prev);
        }
    }
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

void deleteDirectory(char *pathToDir) {
    DIR *dir = opendir(pathToDir);
    struct dirent *entry = NULL;
    while ((entry = readdir(dir)) != NULL) {
        if (strcmp(".", entry->d_name) == 0 ||
            strcmp("..", entry->d_name) == 0) {
            continue;
        }
        char path[BUFFER_SIZE];
        memset(path, 0, BUFFER_SIZE);
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

char *UTF8ToGBK(const char *utf8String) {
    // Map UTF8 string to Unicode
    int bufferSize = MultiByteToWideChar(CP_UTF8, 0, utf8String, -1, NULL, 0);
    wchar_t buffer[bufferSize];
    wmemset(buffer, 0, bufferSize);
    MultiByteToWideChar(CP_UTF8, 0, utf8String, -1, buffer, bufferSize);
    // Map Unicode string to GBK string
    bufferSize = WideCharToMultiByte(CP_ACP, 0, buffer, -1, NULL, 0, NULL, NULL);
    char *result = calloc(bufferSize, 1);
    WideCharToMultiByte(CP_ACP, 0, buffer, -1, result, bufferSize, NULL, NULL);
    return result;
}

char *GBKToUTF8(const char *gbkString) {
    int bufferSize = MultiByteToWideChar(CP_ACP, 0, gbkString, -1, NULL, 0);
    wchar_t buffer[bufferSize];
    wmemset(buffer, 0, bufferSize);
    MultiByteToWideChar(CP_ACP, 0, gbkString, -1, buffer, bufferSize);
    bufferSize = WideCharToMultiByte(CP_UTF8, 0, buffer, -1, NULL, 0, NULL, NULL);
    char *result = calloc(bufferSize, 1);
    WideCharToMultiByte(CP_UTF8, 0, buffer, -1, result, bufferSize, NULL, NULL);
    return result;
}

/**
 * ==================================================================================
 *                            Utility Functions End
 * ==================================================================================
 */

