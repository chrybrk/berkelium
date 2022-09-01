#pragma once

#include "lexer.h"
#include "array.h"
#include "glob.h"

enum
{
    AST_ADD,
    AST_SUB,
    AST_DIV,
    AST_MUL,
    AST_MOD,
    AST_EQU,
    AST_NEQ,
    AST_GT,
    AST_LT,
    AST_GEQ,
    AST_LEQ,
    AST_INTLIT,
    AST_IDENT,
    AST_LVAL,
    AST_ASSIGN,
    AST_PRINT,
    AST_IF,
    AST_WHILE,
    AST_GLUE
};

struct ASTnode
{
    int op;
    struct ASTnode *left;
    struct ASTnode *mid;
    struct ASTnode *right;
    int intvalue;
};

typedef struct PARSER_STRUCT
{
    lexer_T *lexer;
    struct token *token;
} parser_T;

struct ASTnode *init_ASTnode(int op, struct ASTnode *left, struct ASTnode *mid, struct ASTnode *right, int intvalue);
struct ASTnode *ASTnode_leaf(int op, int intvalue);
struct ASTnode *ASTnode_unary(int op, struct ASTnode *left, int intvalue);
int ASTnode_op(struct token *token);

parser_T *init_parser(lexer_T *lexer);
struct token *eat(parser_T *parser, int token);
struct ASTnode *primary(parser_T *parser);
struct ASTnode *parser_parse_expr(parser_T *parser, int tok_prec);
struct ASTnode *parser_parse_assignment(parser_T *parser);
struct ASTnode *parser_parse_compound_statements(parser_T *parser);
struct ASTnode *parser_parse(parser_T *parser);
