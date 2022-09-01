#pragma once

#include "parser.h"
#include "utils.h"

typedef struct CODEGEN_STRUCT
{
    FILE *outfile;
} codegen_T;

codegen_T *init_codegen(char *outfile);
void codegen_code(codegen_T *codegen, struct ASTnode *node);
int genIF(codegen_T *codegen, struct ASTnode *node);
int genAST(codegen_T *codegen, struct ASTnode *node, int reg, int parent_op);

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
int asm_mod(FILE *outfile, int r1, int r2);

int asm_equ(FILE *outfile, int r1, int r2);
int asm_neq(FILE *outfile, int r1, int r2);
int asm_gt(FILE *outfile, int r1, int r2);
int asm_lt(FILE *outfile, int r1, int r2);
int asm_geq(FILE *outfile, int r1, int r2);
int asm_leq(FILE *outfile, int r1, int r2);
int asm_compare_set(FILE *outfile, int op, int r1, int r2);
int asm_compare_jump(FILE *outfile, int op, int r1, int r2, int label);
void asm_label(FILE *outfile, int label);
void asm_jump(FILE *outfile, int label);
void asm_genglob(FILE *outfile, char *sym);
int asm_compare(FILE *outfile, int r1, int r2, char *cond);

// function
void asm_printint(FILE *outfile, int r);
