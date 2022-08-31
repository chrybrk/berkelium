#include "include/codegen.h"
#include "include/macro.h"
#include "include/glob.h"

static char *reg[4] = { "%r8", "%r9", "%r10", "%r11" };
static char *breg[4] = { "%r8b", "%r9b", "%r10b", "%r11b" };
static char *cmplist[] = { "sete", "setne", "setl", "setg", "setle", "setge" };
static char *invcmplist[] = { "jne", "je", "jle", "jge", "jl", "jg" };

int free_reg[4] = { 0 };
int label = 0;

int get_label()
{
    return label++;
}

char *get_cmp(int op, char **from)
{
    if (op < AST_EQU || op > AST_LEQ) log(3, "invalid cmp, `%s`", tok_type_to_string(op));

    switch (op)
    {
        case AST_EQU: return from[0];
        case AST_NEQ: return from[1];
        case AST_LT: return from[2];
        case AST_GT: return from[3];
        case AST_LEQ: return from[4];
        case AST_GEQ: return from[5];
        default: log(3, "%s", "no cmp found");
    }

    return NULL;
}

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
    int reg = genAST(codegen, node, -1, 0);
    postamble(codegen->outfile);
    fclose(codegen->outfile);
}

int genIF(codegen_T *codegen, struct ASTnode *node)
{
    int Lfalse, Lend;

    Lfalse = get_label();
    if (node->right) Lend = get_label();

    genAST(codegen, node->left, Lfalse, node->op);
    reg_freeall();

    genAST(codegen, node->mid, -1, node->op);
    reg_freeall();

    if (node->right) asm_jump(codegen->outfile, Lend);
    asm_label(codegen->outfile, Lfalse);

    if (node->right) {
        genAST(codegen, node->right, -1, node->op);
        reg_freeall();
        asm_label(codegen->outfile, Lend);
    }

    return -1;
}

int genWHILE(codegen_T *codegen, struct ASTnode *node)
{
    int Lstart, Lend;

    Lstart = get_label(); Lend = get_label();
    asm_label(codegen->outfile, Lstart);

    genAST(codegen, node->left, Lend, node->op);
    reg_freeall();

    genAST(codegen, node->right, -1, node->op);
    reg_freeall();

    asm_jump(codegen->outfile, Lstart);
    asm_label(codegen->outfile, Lend);

    return -1;
}

int genAST(codegen_T *codegen, struct ASTnode *node, int reg, int parent_op)
{
    int left, right;

    switch (node->op)
    {
        case AST_IF: return genIF(codegen, node);
        case AST_WHILE: return genWHILE(codegen, node);
        case AST_GLUE: {
                           genAST(codegen, node->left, -1, node->op);
                           reg_freeall();
                           genAST(codegen, node->right, -1, node->op);
                           reg_freeall();

                           return -1;
                       }
    }

    if (node->left) left = genAST(codegen, node->left, -1, node->op);
    if (node->right) right = genAST(codegen, node->right, left, node->op);

    switch (node->op)
    {
        case AST_ADD: return asm_add(codegen->outfile, left, right);
        case AST_SUB: return asm_sub(codegen->outfile, left, right);
        case AST_MUL: return asm_mul(codegen->outfile, left, right);
        case AST_DIV: return asm_div(codegen->outfile, left, right);

        case AST_EQU:
        case AST_NEQ:
        case AST_GT:
        case AST_LT:
        case AST_GEQ:
        case AST_LEQ:
            {
                if (parent_op == AST_IF || parent_op == AST_WHILE) return asm_compare_jump(codegen->outfile, node->op, left, right, reg);
                else return asm_compare_set(codegen->outfile, node->op, left, right);
            }

        case AST_INTLIT: return asm_loadint(codegen->outfile, node->intvalue);
        case AST_IDENT: return asm_loadglob(codegen->outfile, symb_table_find(node->intvalue)->name);
        case AST_LVAL: return asm_storeglob(codegen->outfile, symb_table_find(node->intvalue)->name, reg);
        case AST_ASSIGN: return right;
        case AST_PRINT: asm_printint(codegen->outfile, left); reg_freeall(); return -1;
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

int asm_compare_set(FILE *outfile, int op, int r1, int r2)
{
    fprintf(outfile, "\tcmpq\t%s, %s\n", reg[r2], reg[r1]);
    fprintf(outfile, "\t%s\t%s\n", get_cmp(op, cmplist), breg[r2]);
    fprintf(outfile, "\tmovzbq\t%s, %s\n", breg[r2], reg[r2]);
    reg_free(r1);

    return r2;
}

int asm_compare_jump(FILE *outfile, int op, int r1, int r2, int label)
{
    fprintf(outfile, "\tcmpq\t%s, %s\n", reg[r1], reg[r2]);
    fprintf(outfile, "\t%s\tL%d\n", get_cmp(op, invcmplist), label);
    reg_freeall();

    return -1;
}

void asm_label(FILE *outfile, int label)
{
    fprintf(outfile, "L%d:\n", label);
}

void asm_jump(FILE *outfile, int label)
{
    fprintf(outfile, "\tjmp\tL%d\n", label);
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
