#include "include/glob.h"
#include "include/macro.h"

#include <string.h>

void create_symb_table(char *name, int type, int stype, int label)
{
    symb_table_T *new_symb = calloc(1, sizeof(struct SYMB_TABLE_STRUCT));

    new_symb->name = name;
    new_symb->type = type;
    new_symb->stype = stype;
    new_symb->label = label;

    if (glob_pos < GLOB_SIZE)
    {
        if (symb_table_get(name) >= 0)
        {
            if (symb_table_get_stype(name) == stype) log(3, "GLOBAL_SYMB_TABLE\n\tfound `%s`, cannot redefine", name);
        }
        glob_symb[glob_pos] = new_symb;
        glob_pos++;
    }
    else log(3, "%s", "GLOBAL_SYMB_TABLE\n\tinvalid size");
}

int init_symb_table(char *name)
{
    for (int i = 0; i < glob_pos; i++)
    {
        if (!strcmp(name, glob_symb[i]->name))
        {
            if (glob_init[i] == 0)
            {
                glob_init[i] = 1;
                return 0;
            }
        }
    }

    return 1;
}

int symb_table_get(char *name)
{
    for (int i = 0; i < glob_pos; i++)
    {
        if (!strcmp(name, glob_symb[i]->name))
        {
            // if (glob_init[i] == 0) log(3, "seg fault, `%s` not found", name);
            return i;
        }
    }

    return -1;
}

int symb_table_get_type(char *name)
{
    return symb_table_find(symb_table_get(name))->type;
}

int symb_table_get_stype(char *name)
{
    return symb_table_find(symb_table_get(name))->stype;
}

int symb_table_get_label(int index)
{
    return symb_table_find(index)->label;
}

symb_table_T *symb_table_find(int index)
{
    if (index < glob_pos) return glob_symb[index];
    return NULL;
}
