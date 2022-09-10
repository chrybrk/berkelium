#include "include/codegen.h"
#include "include/macro.h"
#include "include/glob.h"

static char *reg[4] = { "%r8", "%r9", "%r10", "%r11" };
static char *breg[4] = { "%r8b", "%r9b", "%r10b", "%r11b" };
static char *wreg[4] = { "%r8w", "%r9w", "%r10w", "%r11w" };
static char *dreg[4] = { "%r8d", "%r9d", "%r10d", "%r11d" };
static char *x64greg[4] = { "%rax", "%rbx", "%rcx", "%rdx" };
static char *cmplist[] = { "setne", "sete", "setl", "setg", "setle", "setge" };
static char *invcmplist[] = { "jne", "je", "jle", "jge", "jl", "jg" };

int free_reg[4] = { 0 };
int label = 0;

int get_label()
{
    return label++;
}

char *mov_typesize(int type)
{
    switch (type)
    {
        case P_byte: return "movzbq";
        case P_i16: return "movzbl";
        case P_i32: return "movzbl";
        case P_i64: return "movq";
        default: return "invalid_size";
    }
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
        "\n",
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
    // postamble(codegen->outfile);
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
        case AST_FUNCTION:
                        {
                            asm_function_preamble(codegen->outfile, symb_table_find(node->intvalue)->name);
                            int r = genAST(codegen, node->left, -1, node->op);
                            asm_function_postamble(codegen->outfile, node->intvalue);
                            return -1;
                        }
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
        case AST_MOD: return asm_mod(codegen->outfile, left, right);

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
        case AST_WIDE: return asm_wide(left, node->left->type, node->type);
        case AST_RETURN: asm_return(codegen->outfile, left, node->intvalue); return -1;
        case AST_FUNCTION_CALL: return asm_call(codegen->outfile, left, node->intvalue);
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

    int id = symb_table_get(value);
    int type = symb_table_find(id)->type;
    char *typesize = mov_typesize(type);

    switch (symb_table_find(id)->type)
    {
        case P_byte:
            fprintf(outfile, "\tmovzbq\t%s(\%%rip), %s\n", value, reg[r]);
            break;

        case P_i16:
            fprintf(outfile, "\tmovzbq\t%s(\%%rip), %s\n", value, reg[r]);
            break;

        case P_i32:
            fprintf(outfile, "\tmovzbl\t%s(\%%rip), %s\n", value, dreg[r]);
            break;

        case P_i64:
            fprintf(outfile, "\tmovq\t%s(\%%rip), %s\n", value, reg[r]);
            break;
        
        default: log(3, "%s", "invalid size.");
    }

    return r;
}

int asm_storeglob(FILE *outfile, char *name, int r)
{
    int idx = init_symb_table(name);
    if (idx == 0) asm_genglob(outfile, name);

    int id = symb_table_get(name);

    switch (symb_table_find(id)->type)
    {
        case P_byte:
            fprintf(outfile, "\tmovb\t%s, %s(\%%rip)\n", breg[r], name);
            break;

        case P_i16:
            fprintf(outfile, "\tmovw\t%s, %s(\%%rip)\n", wreg[r], name);
            break;

        case P_i32:
            fprintf(outfile, "\tmovl\t%s, %s(\%%rip)\n", dreg[r], name);
            break;

        case P_i64:
            fprintf(outfile, "\tmovq\t%s, %s(\%%rip)\n", reg[r], name);
            break;
        
        default: log(3, "%s", "invalid size.");
    }


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

int asm_mod(FILE *outfile, int r1, int r2)
{
    fprintf(outfile, "\tmovq\t%s,%%rax\n", reg[r1]);
    fprintf(outfile, "\tcltd\n");
    fprintf(outfile, "\tidivq\t%s\n", reg[r2]);
    fprintf(outfile, "\tmovsxd\t%%edx,%s\n", reg[r1]);

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

void asm_function_preamble(FILE *outfile, char *name)
{
    char *template_preamble = "\t.globl\t%s\n"
                              "\t.type\t%s, @function\n"
                              "%s:\n"
                              "\tpushq\t%%rbp\n"
                              "\tmovq\t%%rsp, %%rbp\n";

    fprintf(outfile, template_preamble, name, name, name);
}

void asm_function_postamble(FILE *outfile, int r)
{
    asm_label(outfile, r);
    char *template_postamble = "\tpopq\t%rbp\n"
                               "\tret\n";
    fputs(template_postamble, outfile);
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
    int id = symb_table_get(sym);
    int type = symb_table_find(id)->type;
    int size = get_prem_size(type);

    fprintf(outfile, "\t.comm\t%s,%d,%d\n", sym, size, size);
}

int asm_compare(FILE *outfile, int r1, int r2, char *cond)
{
    fprintf(outfile, "\tcmpq\t%s, %s\n", reg[r2], reg[r1]);
    fprintf(outfile, "\t%s\t%s\n", cond, breg[r2]);
    fprintf(outfile, "\tandq\t$255,%s\n", reg[r2]);
    reg_free(r1);
    return r2;
}

void asm_return(FILE *outfile, int r, int id)
{
    switch (symb_table_find(id)->type)
    {
        case P_byte:
            fprintf(outfile, "\tmovzbl\t%s, %%eax\n", breg[r]);
            break;

        case P_i16:
            fprintf(outfile, "\tmovzbq\t%s, %%eax\n",  wreg[r]);
            break;

        case P_i32:
            fprintf(outfile, "\tmovl\t%s, %%eax\n", dreg[r]);
            break;

        case P_i64:
            fprintf(outfile, "\tmovq\t%s, %%rax\n", reg[r]);
            break;
        
        default: log(3, "%s", "invalid size.");
    }

    asm_jump(outfile, symb_table_get_label(id));
}

int asm_call(FILE *outfile, int r, int id)
{
    int outr = reg_alloc();
    fprintf(outfile, "\tmovq\t%s, %%rdi\n", reg[r]);
    fprintf(outfile, "\tcall\t%s\n", symb_table_find(id)->name);
    fprintf(outfile, "\tmovq\t%%rax, %s\n", reg[outr]);
    reg_free(r);
    return (outr);
}

// function
void asm_printint(FILE *outfile, int r)
{
    fprintf(outfile, "\tmovq\t%s, %%rdi\n", reg[r]);
    fprintf(outfile, "\tcall\tprintint\n");

    reg_free(r);
}

int asm_wide(int r, int o, int n)
{
    return r;
}
