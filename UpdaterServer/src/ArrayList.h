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
    int index;
    char *data;
    size_t elementSize;
    size_t maxCount;
} ArrayList;

ArrayList *arrayListNew(size_t elementSize);

int arrayListAdd(ArrayList *arrayList, const void *element);

int arrayListSize(const ArrayList *arrayList);

int arrayListInsert(ArrayList *arrayList, const void *element, int position);

int arrayListReplace(ArrayList *arrayList, const void *element, int position);

int arrayListRemove(ArrayList *arrayList, int position);

/*
 * Return a pointer to the data at position or NULL if the position is outside the arrayList.
 */
void *arrayListGet(const ArrayList *arrayList, int position);

void arrayListPrintInfo(const ArrayList *arrayList);

/**
 * Sort the arrayList according to the given comparator parameter.
 * @param arrayList ArrayList need to be sorted.
 * @param comparator return -1 if element01 goes before element02,
 *                           0 if element01 is equals to element02,
 *                           1 if element01 goes after element02.
 */
void arrayListSort(ArrayList *arrayList, int (*comparator)(const void *, const void *));

void *arrayListBinarySearch(ArrayList *arrayList, const void *element, int (*comparator)(const void *, const void *));

void arrayListDelete(ArrayList *arrayList);

#endif //DSA_ARRAYLIST_H
