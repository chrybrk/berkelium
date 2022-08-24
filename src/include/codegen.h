#pragma once

#include "parser.h"
#include "utils.h"

typedef struct CODEGEN_STRUCT
{
    FILE *outfile;
} codegen_T;

codegen_T *init_codegen(char *outfile);
void codegen_code(codegen_T *codegen, struct ASTnode *node);
int genAST(codegen_T *codegen, struct ASTnode *node, int reg);

void preamble(FILE *outfile);
void postamble(FILE *outfile);

// some asm related stuff
int reg_alloc();
void reg_free(int ptr);
void reg_freeall();

// some asm functions
int asm_loadint(FILE *outfile, int value);
int asm_loadglob(FILE *outfile, char *value);
int asm_storeglob(FILE *outfile, char *name, int r);
int asm_add(FILE *outfile, int r1, int r2);
int asm_sub(FILE *outfile, int r1, int r2);
int asm_mul(FILE *outfile, int r1, int r2);
int asm_div(FILE *outfile, int r1, int r2);

void asm_genglob(FILE *outfile, char *sym);

// function
void asm_printint(FILE *outfile, int r);
