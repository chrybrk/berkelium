#include "include/codegen.h"
#include "include/macro.h"
#include "include/glob.h"

static char *reg[4] = { "%r8", "%r9", "%r10", "%r11" };
static char *breg[4] = { "%r8b", "%r9b", "%r10b", "%r11b" };
int free_reg[4] = { 0 };

codegen_T *init_codegen(char *outfile)
{
    codegen_T *new_codegen = calloc(1, sizeof(struct CODEGEN_STRUCT));

    new_codegen->outfile = fopen(outfile, "w");
    assert(new_codegen->outfile, "failed to init outfile");

    return new_codegen;
}

void preamble(FILE *outfile)
{
    reg_freeall();
    fputs(
        ".section    .note.GNU-stack,\"\",@progbits\n"
        "\t.text\n"
        ".LC0:\n"
        "\t.string\t\"%d\\n\"\n"
        "printint:\n"
        "\tpushq\t%rbp\n"
        "\tmovq\t%rsp, %rbp\n"
        "\tsubq\t$16, %rsp\n"
        "\tmovl\t%edi, -4(%rbp)\n"
        "\tmovl\t-4(%rbp), %eax\n"
        "\tmovl\t%eax, %esi\n"
        "\tleaq	.LC0(%rip), %rdi\n"
        "\tmovl	$0, %eax\n"
        "\tcall	printf@PLT\n"
        "\tnop\n"
        "\tleave\n"
        "\tret\n"
        "\n"
        "\t.globl\tmain\n"
        "\t.type\tmain, @function\n"
        "main:\n"
        "\tpushq\t%rbp\n"
        "\tmovq	%rsp, %rbp\n",
    outfile);
}

void postamble(FILE *outfile)
{
    fputs(
        "\tmovl	$0, %eax\n"
        "\tpopq	%rbp\n"
        "\tret\n",
    outfile);
}

void codegen_code(codegen_T *codegen, struct ASTnode *node)
{
    preamble(codegen->outfile);
    int reg = genAST(codegen, node, -1);
    postamble(codegen->outfile);
    fclose(codegen->outfile);
}

int genAST(codegen_T *codegen, struct ASTnode *node, int reg)
{
    int left, right;

    switch (node->op)
    {
        case AST_GLUE: {
                           genAST(codegen, node->left, -1);
                           reg_freeall();
                           genAST(codegen, node->right, -1);
                           reg_freeall();

                           return -1;
                       }
    }

    if (node->left) left = genAST(codegen, node->left, -1);
    if (node->right) right = genAST(codegen, node->right, left);

    switch (node->op)
    {
        case AST_ADD: return asm_add(codegen->outfile, left, right);
        case AST_SUB: return asm_sub(codegen->outfile, left, right);
        case AST_MUL: return asm_mul(codegen->outfile, left, right);
        case AST_DIV: return asm_div(codegen->outfile, left, right);
        case AST_EQU: return asm_equ(codegen->outfile, left, right);
        case AST_NEQ: return asm_neq(codegen->outfile, left, right);
        case AST_GT: return asm_gt(codegen->outfile, left, right);
        case AST_LT: return asm_lt(codegen->outfile, left, right);
        case AST_GEQ: return asm_geq(codegen->outfile, left, right);
        case AST_LEQ: return asm_leq(codegen->outfile, left, right);
        case AST_INTLIT: return asm_loadint(codegen->outfile, node->intvalue);
        case AST_IDENT: return asm_loadglob(codegen->outfile, symb_table_find(node->intvalue)->name);
        case AST_LVAL: return asm_storeglob(codegen->outfile, symb_table_find(node->intvalue)->name, reg);
        case AST_ASSIGN: return right;
        case AST_PRINT: asm_printint(codegen->outfile, left); return -1;
        default: log(3, "%s", "Unrecognised ASTnode:Type");
    }

    return 0;
}

// some asm related stuff
int reg_alloc()
{
    int r = -1;
    for (int i = 0; i < 4; i++)
    {
        if (free_reg[i] == 0)
        {
            r = i;
            break;
        }
    }

    assert(!(r == -1), "cannot allocate register");
    free_reg[r] = 1;

    return r;
}

void reg_free(int ptr)
{
    free_reg[ptr] = 0;
}

void reg_freeall()
{
    for (int i = 0; i < 4; i++)
    {
        free_reg[i] = 0;
    }
}

// some asm functions
int asm_loadint(FILE *outfile, int value)
{
    int allocated_reg = reg_alloc();
    fprintf(outfile, "\tmovq\t$%d, %s\n", value, reg[allocated_reg]);

    return allocated_reg;
}

int asm_loadglob(FILE *outfile, char *value)
{
    int r = reg_alloc();
    fprintf(outfile, "\tmovq\t%s(\%%rip), %s\n", value, reg[r]);
    return r;
}

int asm_storeglob(FILE *outfile, char *name, int r)
{
    int idx = init_symb_table(name);
    if (idx == 0) asm_genglob(outfile, name);

    fprintf(outfile, "\tmovq\t%s, %s(\%%rip)\n", reg[r], name);
    return r;
}

int asm_add(FILE *outfile, int r1, int r2)
{
    fprintf(outfile, "\taddq\t%s, %s\n", reg[r1], reg[r2]);
    reg_free(r1);

    return r2;
}

int asm_sub(FILE *outfile, int r1, int r2)
{
    fprintf(outfile, "\tsubq\t%s, %s\n", reg[r2], reg[r1]);
    reg_free(r2);

    return r1;
}

int asm_mul(FILE *outfile, int r1, int r2)
{
    fprintf(outfile, "\timulq\t%s, %s\n", reg[r1], reg[r2]);
    reg_free(r1);

    return r2;
}

int asm_div(FILE *outfile, int r1, int r2)
{
    fprintf(outfile, "\tmovq\t%s,%%rax\n", reg[r1]);
    fprintf(outfile, "\tcqo\n");
    fprintf(outfile, "\tidivq\t%s\n", reg[r2]);
    fprintf(outfile, "\tmovq\t%%rax,%s\n", reg[r1]);

    reg_free(r2);

    return r1;
}

int asm_equ(FILE *outfile, int r1, int r2)
{
    return asm_compare(outfile, r1, r2, "sete");
}

int asm_neq(FILE *outfile, int r1, int r2)
{
    return asm_compare(outfile, r1, r2, "setne");
}

int asm_gt(FILE *outfile, int r1, int r2)
{
    return asm_compare(outfile, r1, r2, "setg");
}

int asm_lt(FILE *outfile, int r1, int r2)
{
    return asm_compare(outfile, r1, r2, "setl");
}

int asm_geq(FILE *outfile, int r1, int r2)
{
    return asm_compare(outfile, r1, r2, "setge");
}

int asm_leq(FILE *outfile, int r1, int r2)
{
    return asm_compare(outfile, r1, r2, "setle");
}

void asm_genglob(FILE *outfile, char *sym)
{
  fprintf(outfile, "\t.comm\t%s,8,8\n", sym);
}

int asm_compare(FILE *outfile, int r1, int r2, char *cond)
{
    fprintf(outfile, "\tcmpq\t%s, %s\n", reg[r2], reg[r1]);
    fprintf(outfile, "\t%s\t%s\n", cond, breg[r2]);
    fprintf(outfile, "\tandq\t$255,%s\n", reg[r2]);
    reg_free(r1);
    return r2;
}

// function
void asm_printint(FILE *outfile, int r)
{
    fprintf(outfile, "\tmovq\t%s, %%rdi\n", reg[r]);
    fprintf(outfile, "\tcall\tprintint\n");

    reg_free(r);
}
