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

typedef struct {
    int _index;
    char *_data;
    size_t _elementSize;
    size_t _maxCount;
} ArrayList;

ArrayList *arrayListNew(size_t elementSize);

int arrayListAdd(ArrayList *arrayList, void *element);

int arrayListSize(ArrayList *arrayList);

int arrayListInsert(ArrayList *arrayList, void *element, int position);

int arrayListReplace(ArrayList *arrayList, void *element, int position);

int arrayListRemove(ArrayList *arrayList, int position);

/*
 * Return a pointer to the data at position or NULL if the position is outside the arrayList.
 */
void *arrayListGet(ArrayList *arrayList, int position);

void arrayListPrintInfo(ArrayList *arrayList);

void arrayListDelete(ArrayList *arrayList);

#endif //DSA_ARRAYLIST_H
