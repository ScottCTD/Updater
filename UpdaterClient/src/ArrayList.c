//
// Created by Scott on 2021/6/14.
//

#include "ArrayList.h"

bool isInRange(ArrayList *arrayList, int index) {
    if (index > arrayList->_index) {
        printf("IndexOutOfBoundException\n");
        return false;
    }
    return true;
}

int arrayListGrow(ArrayList *arrayList) {
    arrayList->_maxCount *= ARRAYLIST_REALLOC_FACTOR;
    if (realloc(arrayList->_data, arrayList->_maxCount * arrayList->_elementSize) == NULL) {
        return ARRAYLIST_FAIL;
    }
    return ARRAYLIST_SUCCESS;
}

int arrayListSize(ArrayList *arrayList) {
    return arrayList->_index + 1;
}

int arrayListAdd(ArrayList *arrayList, void *element) {
    if (arrayList->_index >= arrayList->_maxCount - 1) {
        if (arrayListGrow(arrayList) == ARRAYLIST_FAIL) {
            return ARRAYLIST_FAIL;
        }
    }
    ++arrayList->_index;
    memcpy(arrayList->_data + arrayList->_index * arrayList->_elementSize, element, arrayList->_elementSize);
    return ARRAYLIST_SUCCESS;
}

int arrayListInsert(ArrayList *arrayList, void *element, int position) {
    if (!isInRange(arrayList, position)) {
        return ARRAYLIST_FAIL;
    }
    if (position == arrayList->_index) {
        if (arrayListAdd(arrayList, element) == -1) {
            return ARRAYLIST_FAIL;
        }
    }
    if (++arrayList->_index >= arrayList->_maxCount - 1) {
        if (arrayListGrow(arrayList) == ARRAYLIST_FAIL) {
            return ARRAYLIST_FAIL;
        }
    }
    memcpy(arrayList->_data + (position + 1) * arrayList->_elementSize,
           arrayList->_data + position * arrayList->_elementSize,
           (arrayList->_index - position) * arrayList->_elementSize);
    memcpy(arrayList->_data + position * arrayList->_elementSize, element, arrayList->_elementSize);
    return ARRAYLIST_SUCCESS;
}

int arrayListReplace(ArrayList *arrayList, void *element, int position) {
    if (!isInRange(arrayList, position)) {
        return ARRAYLIST_FAIL;
    }
    memcpy(arrayList->_data + position * arrayList->_elementSize, element, arrayList->_elementSize);
    return ARRAYLIST_SUCCESS;
}

int arrayListRemove(ArrayList *arrayList, int position) {
    if (!isInRange(arrayList, position)) {
        return ARRAYLIST_FAIL;
    }
    memcpy(arrayList->_data + position * arrayList->_elementSize,
           arrayList->_data + (position + 1) * arrayList->_elementSize,
           (arrayList->_index - position) * arrayList->_elementSize);
    --arrayList->_index;
    // Shrink the space
    if (arrayList->_index < arrayList->_maxCount / ARRAYLIST_REALLOC_FACTOR - 1) {
        arrayList->_maxCount /= ARRAYLIST_REALLOC_FACTOR;
        if (realloc(arrayList->_data, arrayList->_maxCount * arrayList->_elementSize) == NULL) {
            return ARRAYLIST_FAIL;
        }
    }
    return ARRAYLIST_SUCCESS;
}

/*
 * Return a pointer to the data at position or NULL if the position is outside the arrayList.
 */
void *arrayListGet(ArrayList *arrayList, int position) {
    if (!isInRange(arrayList, position)) {
        return NULL;
    }
    return arrayList->_data + position * arrayList->_elementSize;
}

void arrayListPrintInfo(ArrayList *arrayList) {
    printf("Size = %d, Element Size = %zu, Internal Index = %d, Max Count = %zu\n",
           arrayListSize(arrayList), arrayList->_elementSize, arrayList->_index, arrayList->_maxCount);
}

void arrayListDelete(ArrayList *arrayList) {
    free(arrayList->_data);
    free(arrayList);
}

ArrayList *arrayListNew(size_t elementSize)  {
    ArrayList *arrayList = malloc(sizeof(ArrayList));
    arrayList->_data = calloc(ARRAYLIST_INITIAL_COUNT, elementSize);
    arrayList->_elementSize = elementSize;
    arrayList->_maxCount = ARRAYLIST_INITIAL_COUNT;
    arrayList->_index = -1;
    return arrayList;
}