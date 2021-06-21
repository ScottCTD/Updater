//
// Created by Scott on 2021/6/14.
//

#ifndef DSA_ARRAYLIST_H
#define DSA_ARRAYLIST_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

#define ARRAYLIST_FAIL (-1)
#define ARRAYLIST_SUCCESS (0)

#define ARRAYLIST_INITIAL_COUNT (10)
#define ARRAYLIST_REALLOC_FACTOR (2)

typedef struct _ArrayList ArrayList;

struct _ArrayList {
    int _index;
    char *_data;
    size_t _elementSize;
    size_t _maxCount;

    int (*size)(ArrayList *arrayList);
    int (*add)(ArrayList *arrayList, void *element);
    int (*insert)(ArrayList *arrayList, void *element, int position);
    int (*replace)(ArrayList *arrayList, void *element, int position);
    int (*remove)(ArrayList *arrayList, int position);
    void *(*get)(ArrayList *arrayList, int position);
    void (*printInfo)(ArrayList *arrayList);
    void (*delete)(ArrayList *arrayList);
};

ArrayList *arrayListNew(size_t elementSize);

#endif //DSA_ARRAYLIST_H
