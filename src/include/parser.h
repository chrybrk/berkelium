#pragma once

#include "lexer.h"

enum
{
    AST_ADD,
    AST_SUB,
    AST_DIV,
    AST_MUL,
    AST_INTLIT
};

struct ASTnode
{
    int op;
    struct ASTnode *left;
    struct ASTnode *right;
    int intvalue;
};

typedef struct PARSER_STRUCT
{
    lexer_T *lexer;
    struct token *token;
} parser_T;

struct ASTnode *init_ASTnode(int op, struct ASTnode *left, struct ASTnode *right, int intvalue);
struct ASTnode *ASTnode_leaf(int op, int intvalue);
struct ASTnode *ASTnode_unary(int op, struct ASTnode *left, int intvalue);
int ASTnode_op(struct token *token);


parser_T *init_parser(lexer_T *lexer);
struct ASTnode *primary(parser_T *parser);
struct ASTnode *parser_parse(parser_T *parser, int tok_prec);
void parser_parse_statements(parser_T *parser, char *outfile);
