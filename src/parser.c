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
        default: log(3, "ln:%d:%d\n\tsyntax err", parser->token->ln, parser->token->clm);
    }

    return 0;
}

struct ASTnode *parser_parse(parser_T *parser, int tok_prec)
{
    struct ASTnode *left, *right;
    struct token *tok;

    left = primary(parser);

    tok = parser->token;
    if (tok->token == T_SEMI || tok->token == T_RPAREN)
        return left;

    while (ASTnode_op_prec(parser->token) > tok_prec)
    {
        parser->token = lexer_next_token(parser->lexer);

        right = parser_parse(parser, ASTnode_op_prec(tok));
        left = init_ASTnode(ASTnode_op(tok), left, right, 0);

        tok = parser->token;
        if (tok->token == T_SEMI || tok->token == T_RPAREN)
            return left;
    }

    return left;
}

void expected_tok(parser_T *parser, int token)
{
    if (parser->token->token != token) log(3, "ln:%d:%d\n\tsyntax err, expected: `%s` got `%s`", parser->token->ln, parser->token->clm, tok_type_to_string(token), tok_type_to_string(parser->token->token));
    parser->token = lexer_next_token(parser->lexer);
}

void parser_parse_statements(parser_T *parser, char *outfile)
{
    codegen_T *codegen = init_codegen(outfile);
    preamble(codegen->outfile);

    struct ASTnode *tree;

    while (1)
    {
        switch (parser->token->token)
        {
            case T_PRINT:
                {
                    parser->token = lexer_next_token(parser->lexer);
                    expected_tok(parser, T_LPAREN);
                    tree = parser_parse(parser, 0);
                    int reg = genAST(codegen, tree);
                    asm_printint(codegen->outfile, reg);
                    expected_tok(parser, T_RPAREN);
                    break;
                }
            default: log(3, "ln:%d:%d\n\tsyntax err, tok: %s", parser->token->ln, parser->token->clm, tok_string(parser->token));
        }

        if (parser->token->token == T_SEMI) parser->token = lexer_next_token(parser->lexer);

        if (parser->token->token == T_EOF) break;
    }

    postamble(codegen->outfile);
    fclose(codegen->outfile);
}
