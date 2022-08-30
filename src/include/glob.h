#pragma once

typedef struct SYMB_TABLE_STRUCT
{
    char *name;
} symb_table_T;

#define GLOB_SIZE 1024

static symb_table_T *glob_symb[GLOB_SIZE];
static int glob_init[GLOB_SIZE] = { 0 };
static int glob_pos = 0;

void create_symb_table(char *name);
void init_symb_table(char *name);
int symb_table_get(char *name);
symb_table_T *symb_table_find(int index);
