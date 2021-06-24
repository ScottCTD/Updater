//
// Created by Scott on 2021/6/14.
//

#include "ArrayList.h"

bool isInRange(const ArrayList *arrayList, int index) {
    if (index > arrayList->index) {
        printf("IndexOutOfBoundException\n");
        return false;
    }
    return true;
}

int arrayListGrow(ArrayList *arrayList) {
    arrayList->maxCount *= ARRAYLIST_REALLOC_FACTOR;
    if (realloc(arrayList->data, arrayList->maxCount * arrayList->elementSize) == NULL) {
        return ARRAYLIST_FAIL;
    }
    return ARRAYLIST_SUCCESS;
}

int arrayListSize(const ArrayList *arrayList) {
    return arrayList->index + 1;
}

int arrayListAdd(ArrayList *arrayList,  const void *element) {
    if (arrayList->index >= 0 && arrayList->index >= arrayList->maxCount - 1) {
        if (arrayListGrow(arrayList) == ARRAYLIST_FAIL) {
            return ARRAYLIST_FAIL;
        }
    }
    ++arrayList->index;
    memcpy(arrayList->data + arrayList->index * arrayList->elementSize, element, arrayList->elementSize);
    return ARRAYLIST_SUCCESS;
}

int arrayListInsert(ArrayList *arrayList,const void *element, int position) {
    if (!isInRange(arrayList, position)) {
        return ARRAYLIST_FAIL;
    }
    if (position == arrayList->index) {
        if (arrayListAdd(arrayList, element) == -1) {
            return ARRAYLIST_FAIL;
        }
    }
    if (++arrayList->index >= arrayList->maxCount - 1) {
        if (arrayListGrow(arrayList) == ARRAYLIST_FAIL) {
            return ARRAYLIST_FAIL;
        }
    }
    memcpy(arrayList->data + (position + 1) * arrayList->elementSize,
           arrayList->data + position * arrayList->elementSize,
           (arrayList->index - position) * arrayList->elementSize);
    memcpy(arrayList->data + position * arrayList->elementSize, element, arrayList->elementSize);
    return ARRAYLIST_SUCCESS;
}

int arrayListReplace(ArrayList *arrayList, const void *element, int position) {
    if (!isInRange(arrayList, position)) {
        return ARRAYLIST_FAIL;
    }
    memcpy(arrayList->data + position * arrayList->elementSize, element, arrayList->elementSize);
    return ARRAYLIST_SUCCESS;
}

int arrayListRemove(ArrayList *arrayList, int position) {
    if (!isInRange(arrayList, position)) {
        return ARRAYLIST_FAIL;
    }
    memcpy(arrayList->data + position * arrayList->elementSize,
           arrayList->data + (position + 1) * arrayList->elementSize,
           (arrayList->index - position) * arrayList->elementSize);
    --arrayList->index;
    // Shrink the space
    if (arrayList->index < arrayList->maxCount / ARRAYLIST_REALLOC_FACTOR - 1) {
        arrayList->maxCount /= ARRAYLIST_REALLOC_FACTOR;
        if (realloc(arrayList->data, arrayList->maxCount * arrayList->elementSize) == NULL) {
            return ARRAYLIST_FAIL;
        }
    }
    return ARRAYLIST_SUCCESS;
}

/*
 * Return a pointer to the data at position or NULL if the position is outside the arrayList.
 */
void *arrayListGet(const ArrayList *arrayList, int position) {
    if (!isInRange(arrayList, position)) {
        return NULL;
    }
    return arrayList->data + position * arrayList->elementSize;
}

void arrayListSort(ArrayList *arrayList, int (*comparator)(const void *, const void *)) {
    qsort(arrayList->data, arrayListSize(arrayList), arrayList->elementSize, comparator);
}

void *arrayListBinarySearch(ArrayList *arrayList, const void *element, int (*comparator)(const void *, const void *)) {
    return bsearch(element, arrayList->data, arrayListSize(arrayList), arrayList->elementSize, comparator);
}

void arrayListPrintInfo(const ArrayList *arrayList) {
    printf("Size = %d, Element Size = %zu, Internal Index = %d, Max Count = %zu\n",
           arrayListSize(arrayList), arrayList->elementSize, arrayList->index, arrayList->maxCount);
}

void arrayListDelete(ArrayList *arrayList) {
    free(arrayList->data);
    free(arrayList);
    arrayList = NULL;
}

ArrayList *arrayListNew(size_t elementSize)  {
    ArrayList *arrayList = malloc(sizeof(ArrayList));
    arrayList->data = calloc(ARRAYLIST_INITIAL_COUNT, elementSize);
    arrayList->elementSize = elementSize;
    arrayList->maxCount = ARRAYLIST_INITIAL_COUNT;
    arrayList->index = -1;
    return arrayList;
}