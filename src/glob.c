#include "include/glob.h"
#include "include/macro.h"

#include <string.h>

void init_symb_table(char *name)
{
    symb_table_T *new_symb = calloc(1, sizeof(struct SYMB_TABLE_STRUCT));
    new_symb->name = name;

    if (glob_pos < GLOB_SIZE)
    {
        if (symb_table_get(name) >= 0) log(3, "GLOBAL_SYMB_TABLE\n\tfound `%s`, cannot redefine", name);
        glob_symb[glob_pos] = new_symb;
        glob_pos++;
    }
    else log(3, "%s", "GLOBAL_SYMB_TABLE\n\tinvalid size");
}

int symb_table_get(char *name)
{
    for (int i = 0; i < glob_pos; i++)
    {
        if (!strcmp(name, glob_symb[i]->name)) return i;
    }

    return -1;
}

symb_table_T *symb_table_find(int index)
{
    if (index < glob_pos) return glob_symb[index];
    return NULL;
}
