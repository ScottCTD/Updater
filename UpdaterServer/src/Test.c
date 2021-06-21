//
// Created by Scott on 2021/6/19.
//

#include <stdio.h>
#include "cJSON.h"

int main() {

    cJSON *object = cJSON_CreateObject();
    cJSON *fileArray = cJSON_AddArrayToObject(object, "Directories");

    char *json = cJSON_Print(object);
    printf("%s\n", json);

    cJSON_Delete(object);
    return 0;
}