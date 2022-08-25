#include "include/array.h"
#include "include/macro.h"

list_T *init_list(size_t item_size)
{
    list_T *new_list = calloc(1, sizeof(struct LIST_STRUCT));

    new_list->item_size = item_size;
    new_list->item = 0;
    new_list->size = 0;

    return new_list;
}

void list_push(list_T *list, void *item)
{
    list->size++;

    if (list->item)
        list->item = realloc(list->item, list->item_size * list->size);
    else
        list->item = calloc(1, list->item_size * list->size);

    list->item[list->size - 1] = item;
}

void *list_pop_front(list_T *list)
{
    void *del_item = list->item[0];
    list->item++; list->size--;

    return del_item;
}

void *list_pop_back(list_T *list)
{
    void *del_item = list->item[list->size - 1];
    list->item--; list->size--;

    return del_item;
}

void *list_get(list_T *list, int index)
{
    if (index >= list->size) log(3, "%s", "list: invalid size");
    return list->item[index];
}

map_T *init_map()
{
    map_T *new_map = calloc(1, sizeof(char));
    new_map->map_items = init_list(sizeof(struct _key_pair));
    new_map->size = new_map->map_items->size;

    return new_map;
}

void map_push(map_T *map, void *key, void *value)
{
    struct _key_pair *new_map_item = calloc(1, sizeof(struct _key_pair));
    new_map_item->key = key;
    new_map_item->value = value;

    list_push(map->map_items, new_map_item);
    map->size++;
}

void *map_get(map_T *map, void *key)
{
    for (unsigned int i = 0; i < map->size; i++)
    {
        struct _key_pair *map_item = (struct _key_pair*)map->map_items->item[i];
        if (map_item->key == key) return map_item->value;
    }

    return NULL;
}

void *map_bucket(map_T *map, unsigned int index)
{
    if (index >= map->size) log(3, "%s", "map: invalid size");
    return map->map_items->item[index];
}

void *map_pop_back(map_T *map)
{
    return list_pop_back(map->map_items);
}

void *map_pop_front(map_T *map)
{
    return list_pop_front(map->map_items);
}
