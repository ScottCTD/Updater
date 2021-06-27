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

#include "dirent.h"
#include "cJSON.h"


/**
 * ==================================================================================
 *                                 LocalFile Starts
 * ==================================================================================
 */
typedef struct LocalFile{
    bool isDirectory;
    size_t size;
    char *name;
    char *path;
    char *identifier;
    struct LocalFile *prev;
    struct LocalFile *next;
    struct LocalFile *child;
} LocalFile;

LocalFile *newLocalFile(const char *path);

void createLocalFileTree(LocalFile *startDir);

LocalFile *searchLocalFileInFileTreeWithComparator(LocalFile *fileTreeStart, const void *element,
                                                   int (*comparator)(const LocalFile *, const void *));

void freeLocalFile(LocalFile *file);

/**
 * Generate a unique identifier of this file based on name and file size;
 * TODO Use Sha1
 * @param file
 * @return The malloced string represent the identifier.
 *         Should be freed manually.
 */
char *generateLocalFileUniqueIdentifier(LocalFile *file);

cJSON *localFileToJson(LocalFile *file);

LocalFile *localFileFromJson(cJSON *jsonObj);

int localFileComparator(const void *file01, const void *file02);

/**
 * ==================================================================================
 *                                 LocalFile Ends
 * ==================================================================================
 */

/**
 * ==================================================================================
 *                                 RemoteFile Starts
 * ==================================================================================
 */
typedef struct {
    char *path;
    char *name;
    char *identifier;
} RemoteFile;

RemoteFile *remoteFileFromJson(cJSON *json);

void freeRemoteFile(RemoteFile *file);

int remoteFileComparator(const void *file01, const void *file02);

/**
 * ==================================================================================
 *                                 RemoteFile Ends
 * ==================================================================================
 */

/**
 * ==================================================================================
 *                            Utility Functions Start
 * ==================================================================================
 */

void mkdirs(const char *path);
void deleteDirectory(char *pathToDir);
char *UTF8ToGBK(char *utf8String);

/**
 * ==================================================================================
 *                            Utility Functions End
 * ==================================================================================
 */



#endif //UPDATERSERVER_FILESYSTEM_H
