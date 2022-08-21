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
    if (index >= list->size) log(3, "%s", "list:\n\tseg fault");
    return list->item[index];
}
