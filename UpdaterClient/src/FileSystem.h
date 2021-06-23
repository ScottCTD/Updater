//
// Created by Scott on 2021/6/21.
//

#ifndef UPDATERSERVER_FILESYSTEM_H
#define UPDATERSERVER_FILESYSTEM_H

#ifndef BUFFER_SIZE
#define BUFFER_SIZE (8192)
#endif

#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>

#include "cJSON.h"
#include "dirent.h"

typedef struct {
    bool isDirectory;
    size_t size;
    char *name;
    char *path;
} File;

File *newFile(const char *path);

void freeFile(File *file);

/**
 * Generate a unique identifier of this file based on name and file size;
 * TODO Use Sha1
 * @param file
 * @return The malloced string represent the identifier.
 *         Should be freed manually.
 */
char *generateFileUniqueIdentifier(File *file);

cJSON *fileToJson(File *file);

File *fileFromJson(cJSON *jsonObj);

#endif //UPDATERSERVER_FILESYSTEM_H
