#pragma once

#include "parser.h"
#include "utils.h"

typedef struct CODEGEN_STRUCT
{
    FILE *outfile;
} codegen_T;

codegen_T *init_codegen(char *outfile);
void codegen_code(codegen_T *codegen, struct ASTnode *node);
int genAST(codegen_T *codegen, struct ASTnode *node);

// some asm related stuff
int reg_alloc();
void reg_free(int ptr);
void reg_freeall();

// some asm functions
int asm_load(FILE *outfile, int value);
int asm_add(FILE *outfile, int r1, int r2);
int asm_sub(FILE *outfile, int r1, int r2);
int asm_mul(FILE *outfile, int r1, int r2);
int asm_div(FILE *outfile, int r1, int r2);

// function
void asm_printint(FILE *outfile, int r);