#pragma once

enum
{
    S_VARIABLE,
    S_FUNCTION
};

typedef struct SYMB_TABLE_STRUCT
{
    char *name;
    int type;
    int stype;
} symb_table_T;

#define GLOB_SIZE 1024

static symb_table_T *glob_symb[GLOB_SIZE];
static int glob_init[GLOB_SIZE] = { 0 };
static int glob_pos = 0;

void create_symb_table(char *name, int type, int stype);
int init_symb_table(char *name);
int symb_table_get(char *name);
int symb_table_get_type(char *name);
int symb_table_get_stype(char *name);
symb_table_T *symb_table_find(int index);
