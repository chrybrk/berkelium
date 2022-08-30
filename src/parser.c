#include "include/parser.h"
#include "include/macro.h"
#include "include/codegen.h"

struct ASTnode *init_ASTnode(int op, struct ASTnode *left, struct ASTnode *right, int intvalue)
{
    struct ASTnode *new_ASTnode = calloc(1, sizeof(struct ASTnode));

    new_ASTnode->op = op;
    new_ASTnode->left = left;
    new_ASTnode->right = right;
    new_ASTnode->intvalue = intvalue;

    return new_ASTnode;
}

struct ASTnode *ASTnode_leaf(int op, int intvalue)
{
    return (init_ASTnode(op, NULL, NULL, intvalue));
}

struct ASTnode *ASTnode_unary(int op, struct ASTnode *left, int intvalue)
{
    return (init_ASTnode(op, left, NULL, intvalue));
}

int ASTnode_op(struct token *token)
{
    switch (token->token)
    {
        case T_PLUS: return AST_ADD;
        case T_MINUS: return AST_SUB;
        case T_STAR: return AST_MUL;
        case T_SLASH: return AST_DIV;
        case T_EQU: return AST_EQU;
        case T_NEQ: return AST_NEQ;
        case T_GT: return AST_GT;
        case T_LT: return AST_LT;
        case T_GEQ: return AST_GEQ;
        case T_LEQ: return AST_LEQ;
        default: log(3, "Unrecognised token `%s`", tok_string(token));
    }

    // to remove warning
    return 0;
}

int ASTnode_op_prec(struct token *token)
{
    switch (token->token)
    {
        case T_PLUS:
        case T_MINUS:
            return 10;

        case T_STAR:
        case T_SLASH:
            return 20;

        case T_EQU:
        case T_NEQ:
            return 30;
        
        case T_LT:
        case T_GT:
        case T_LEQ:
        case T_GEQ:
            return 40;

        default: log(3, "ln:%d:%d\n\tsyntax err, tok: %s", token->ln, token->clm, tok_string(token));
    }

    return 0;
}

parser_T *init_parser(lexer_T *lexer)
{
    parser_T *new_parser = calloc(1, sizeof(struct PARSER_STRUCT));

    new_parser->lexer = lexer;
    new_parser->token = lexer_next_token(lexer);

    return new_parser;
}

struct ASTnode *primary(parser_T *parser)
{
    switch (parser->token->token)
    {
        case T_INTLIT:
            {
                struct ASTnode *node = ASTnode_leaf(AST_INTLIT, atoi(parser->token->value));
                parser->token = lexer_next_token(parser->lexer);

                return node;
            }
        case T_IDENT:
            {
                int id = symb_table_get(parser->token->value);

                if (id == -1)
                {
                    log(3, "ln:%d:%d\n\tUndefined variable `%s`", parser->token->ln, parser->token->clm - 1, parser->token->value);
                }
                struct ASTnode *node = ASTnode_leaf(AST_IDENT, id);
                eat(parser, T_IDENT);

                return node;
            }
        default: log(3, "ln:%d:%d\n\tsyntax err", parser->token->ln, parser->token->clm);
    }

    return 0;
}

struct ASTnode *parser_parse_expr(parser_T *parser, int tok_prec)
{
    struct ASTnode *left, *right;
    struct token *tok;

    left = primary(parser);

    tok = parser->token;
    if (tok->token == T_SEMI || tok->token == T_RPAREN)
        return left;

    while (ASTnode_op_prec(tok) > tok_prec)
    {
        parser->token = lexer_next_token(parser->lexer);

        right = parser_parse_expr(parser, ASTnode_op_prec(tok));
        left = init_ASTnode(ASTnode_op(tok), left, right, 0);

        tok = parser->token;
        if (tok->token == T_SEMI || tok->token == T_RPAREN)
            return left;
    }

    return left;
}

struct token *eat(parser_T *parser, int token)
{
    if (parser->token->token != token) log(3, "ln:%d:%d\n\tsyntax err, expected: `%s` got `%s`", parser->token->ln, parser->token->clm, tok_type_to_string(token), tok_type_to_string(parser->token->token));
    struct token *to_return = parser->token;
    parser->token = lexer_next_token(parser->lexer);
    return to_return;
}

void expected_tok(parser_T *parser, int token)
{
    if (parser->token->token != token) log(3, "ln:%d:%d\n\tsyntax err, expected: `%s` got `%s`", parser->token->ln, parser->token->clm - 1, tok_type_to_string(token), tok_type_to_string(parser->token->token));
    parser->token = lexer_next_token(parser->lexer);
}

struct ASTnode *parser_parse_print(parser_T *parser)
{
    struct ASTnode *tree;

    eat(parser, T_PRINT);
    expected_tok(parser, T_LPAREN);

    tree = parser_parse_expr(parser, 0);
    // int reg = genAST(codegen, tree, -1);

    // asm_printint(codegen->outfile, reg);
    expected_tok(parser, T_RPAREN);
    expected_tok(parser, T_SEMI);

    return ASTnode_unary(AST_PRINT, tree, 0);
}

struct ASTnode *parser_parse_variable_decl(parser_T *parser)
{
    eat(parser, i32);

    int clm = parser->lexer->clm;
    int ln = parser->lexer->ln;
    char c = parser->lexer->c;
    unsigned int i = parser->lexer->i;
    struct token *tok = parser->token;

    char *ident = calloc(1, sizeof(strlen(parser->token->value)));
    strcpy(ident, parser->token->value);
    eat(parser, T_IDENT);

    create_symb_table(ident);
    // asm_genglob(codegen->outfile, ident);

    if (parser->token->token == T_SEMI)
        eat(parser, T_SEMI);
    else
    {
        parser->lexer->clm = clm;
        parser->lexer->ln = ln;
        parser->lexer->c = c;
        parser->lexer->i = i;
        parser->token = tok;
    }

    return NULL;
}

struct ASTnode *parser_parse_assignment(parser_T *parser)
{
    struct ASTnode *left, *right, *tree;
    init_symb_table(parser->token->value);
    int id = symb_table_get(parser->token->value);

    if (id == -1)
    {
        log(3, "ln:%d:%d\n\tUndefined variable `%s`", parser->token->ln, parser->token->clm - 1, parser->token->value);
    }

    right = ASTnode_leaf(AST_LVAL, id);
    eat(parser, T_IDENT);
    expected_tok(parser, T_ASSIGN);

    left = parser_parse_expr(parser, 0);
    tree = init_ASTnode(AST_ASSIGN, left, right, 0);
    // genAST(codegen, tree, -1);
    // reg_freeall();
    expected_tok(parser, T_SEMI);

    return tree;
}

struct ASTnode *parser_parse_statement(parser_T *parser)
{
    struct ASTnode *tree;

    switch (parser->token->token)
    {
        case T_PRINT: tree = parser_parse_print(parser); break;
        case i32: tree = parser_parse_variable_decl(parser); break;
        case T_IDENT: tree = parser_parse_assignment(parser); break;
        default: tree = parser_parse_expr(parser, 0);
    }

    return tree;
}

struct ASTnode *parser_parse_compound_statements(parser_T *parser)
{
    struct ASTnode *tree;
    struct ASTnode *left = NULL;

    eat(parser, T_LBRACE);

    while (parser->token->token != T_RBRACE)
    {
        tree = parser_parse_statement(parser);

        if (tree)
        {
            if (left == NULL) left = tree;
            else init_ASTnode(AST_GLUE, left, tree, 0);
        }
    }

    eat(parser, T_RBRACE);

    return left;
}

struct ASTnode *parser_parse(parser_T *parser)
{
    struct ASTnode *tree;
    struct ASTnode *left = NULL;

    while (parser->token->token != T_EOF)
    {
        switch (parser->token->token)
        {
            case T_LBRACE: tree = parser_parse_compound_statements(parser); break;
            default: tree = parser_parse_statement(parser); break;
        }

        if (tree)
        {
            if (left == NULL) left = tree;
            else init_ASTnode(AST_GLUE, left, tree, 0);
        }
    }

    return left;
}
