#include "../src/include/array.h"

#include <stdio.h>

typedef struct NUMBER
{
    int value;
} number_T;

int main(void)
{
    list_T *list = init_list(sizeof(struct NUMBER));

    number_T *new_number = calloc(1, sizeof(struct NUMBER));
    new_number->value = 10;

    number_T *new_number2 = calloc(1, sizeof(struct NUMBER));
    new_number2->value = 210;

    list_push(list, new_number);
    list_push(list, new_number2);

    number_T *allocated_value = (struct NUMBER*)list_pop_front(list);
    printf("%d\n", allocated_value->value);


    return 0;
}
