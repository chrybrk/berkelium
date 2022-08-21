#pragma once

#include <stdlib.h>

typedef struct LIST_STRUCT
{
    void **item;
    size_t item_size;
    size_t size;
} list_T;

list_T *init_list(size_t item_size);

void list_push(list_T *list, void *item);

void *list_pop_front(list_T *list);

void *list_pop_back(list_T *list);

void *list_get(list_T *list, int index);
