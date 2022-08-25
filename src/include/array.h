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

typedef struct MAP_STRUCT
{
    list_T *map_items;
    size_t size;
} map_T;

struct _key_pair
{
    void *key;
    void *value;
};

map_T *init_map();
void map_push(map_T *map, void *key, void *value);
void *map_get(map_T *map, void *key);
void *map_bucket(map_T *map, unsigned int index);
void *map_pop_back(map_T *map);
void *map_pop_front(map_T *map);
