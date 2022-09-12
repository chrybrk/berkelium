#include "include/parser.h"
#include "include/macro.h"
#include "include/codegen.h"

struct ASTnode *init_ASTnode(int op, int type, struct ASTnode *left, struct ASTnode *mid, struct ASTnode *right, int intvalue)
{
    struct ASTnode *new_ASTnode = calloc(1, sizeof(struct ASTnode));

    new_ASTnode->op = op;
    new_ASTnode->type = type;
    new_ASTnode->left = left;
    new_ASTnode->mid = mid;
    new_ASTnode->right = right;
    new_ASTnode->intvalue = intvalue;

    return new_ASTnode;
}

struct ASTnode *ASTnode_leaf(int op, int type, int intvalue)
{
    return (init_ASTnode(op, type, NULL, NULL, NULL, intvalue));
}

struct ASTnode *ASTnode_unary(int op, int type, struct ASTnode *left, int intvalue)
{
    return (init_ASTnode(op, type, left, NULL, NULL, intvalue));
}

int ASTnode_op(struct token *token)
{
    switch (token->token)
    {
        case T_PLUS: return AST_ADD;
        case T_MINUS: return AST_SUB;
        case T_STAR: return AST_MUL;
        case T_SLASH: return AST_DIV;
        case T_MODULO: return AST_MOD;
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
        case T_MODULO:
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
                int value = atoi(parser->token->value);
                struct ASTnode *node;

                node = ASTnode_leaf(AST_INTLIT, P_nil, value);

                parser->token = lexer_next_token(parser->lexer);

                return node;
            }
        case T_IDENT:
            {
                int id = symb_table_get(parser->token->value);
                if (symb_table_find(id)->stype == S_FUNCTION) return parser_parse_call(parser);

                if (id == -1)
                {
                    log(3, "ln:%d:%d\n\tUndefined variable `%s`", parser->token->ln, parser->token->clm - 1, parser->token->value);
                }
                struct ASTnode *node = ASTnode_leaf(AST_IDENT, symb_table_find(id)->type, id);
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

        int leftype = left->type;
        int rightype = right->type;

        // if (!type_check(&leftype, &rightype, 0))
            // log(3, "%s", "incompatible type.");
        // if (leftype) left = ASTnode_unary(leftype, right->type, left, 0);
        // if (rightype) right = ASTnode_unary(rightype, left->type, right, 0);

        left = init_ASTnode(ASTnode_op(tok), left->type, left, NULL, right, 0);

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

    expected_tok(parser, T_RPAREN);
    expected_tok(parser, T_SEMI);

    return ASTnode_unary(AST_PRINT, P_nil, tree, 0);
}

struct ASTnode *parser_parse_variable_decl(parser_T *parser)
{
    int type = get_prem_type(eat(parser, parser->token->token)->token);

    if (type == P_void) log(3, "%s\n", "invalid variable type `void`");

    int clm = parser->lexer->clm;
    int ln = parser->lexer->ln;
    char c = parser->lexer->c;
    unsigned int i = parser->lexer->i;
    struct token *tok = parser->token;

    char *ident = calloc(1, sizeof(strlen(parser->token->value)));
    strcpy(ident, parser->token->value);
    eat(parser, T_IDENT);

    create_symb_table(ident, type, S_VARIABLE, 0);
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
    int id = symb_table_get(parser->token->value);

    if (id == -1)
    {
        log(3, "ln:%d:%d\n\tUndefined variable `%s`", parser->token->ln, parser->token->clm - 1, parser->token->value);
    }

    right = ASTnode_leaf(AST_LVAL, symb_table_find(id)->type, id);
    eat(parser, T_IDENT);
    expected_tok(parser, T_ASSIGN);

    left = parser_parse_expr(parser, 0);
    // left->type = symb_table_find(id)->type;

    left->type = right->type;

    int leftype = left->type; int rightype = right->type;
    type_check(leftype, rightype);
    // if (leftype) left = ASTnode_unary(leftype, right->type, left, 0);
    tree = init_ASTnode(AST_ASSIGN, symb_table_find(id)->type, left, NULL, right, 0);

    if (parser->token->token != T_RPAREN) expected_tok(parser, T_SEMI);

    return tree;
}

struct ASTnode *parser_parse_if(parser_T *parser)
{
    struct ASTnode *condtionalAST, *ifAST, *elseAST;

    eat(parser, T_IF);
    eat(parser, T_LPAREN);

    condtionalAST = parser_parse_expr(parser, 0);
    if (condtionalAST->op < AST_EQU || condtionalAST->op > AST_LEQ) log(3, "invalid cmp, `%s`", tok_type_to_string(condtionalAST->op));
    eat(parser, T_RPAREN);

    ifAST = parser_parse_compound_statements(parser);
    if (parser->token->token == T_ELSE)
    {
        eat(parser, T_ELSE);
        elseAST = parser_parse_compound_statements(parser);
    }
    else elseAST = NULL;

    return init_ASTnode(AST_IF, P_nil, condtionalAST, ifAST, elseAST, 0);
}

struct ASTnode *parser_parse_while(parser_T *parser)
{
    struct ASTnode *condtionalAST, *bodyAST;

    eat(parser, T_WHILE);
    eat(parser, T_LPAREN);

    condtionalAST = parser_parse_expr(parser, 0);
    if (condtionalAST->op < AST_EQU || condtionalAST->op > AST_LEQ) log(3, "invalid cmp, `%s`", tok_type_to_string(condtionalAST->op));
    eat(parser, T_RPAREN);

    bodyAST = parser_parse_compound_statements(parser);

    return init_ASTnode(AST_WHILE, P_nil, condtionalAST, NULL, bodyAST, 0);
}

struct ASTnode *parser_parse_for(parser_T *parser)
{
    struct ASTnode *condtionalAST, *bodyAST = NULL;
    struct ASTnode *preopAST, *postopAST = NULL;
    struct ASTnode *tree = NULL;

    eat(parser, T_FOR);
    eat(parser, T_LPAREN);

    parser_parse_statement(parser);
    preopAST = parser_parse_statement(parser);
    condtionalAST = parser_parse_expr(parser, 0);
    expected_tok(parser, T_SEMI);
    if (condtionalAST->op < AST_EQU || condtionalAST->op > AST_LEQ) log(3, "invalid cmp, `%s`", tok_type_to_string(condtionalAST->op));
    postopAST = parser_parse_statement(parser);
    eat(parser, T_RPAREN);

    bodyAST = parser_parse_compound_statements(parser);
    bodyAST = (bodyAST == NULL) ? ASTnode_unary(AST_NOOP, P_nil, bodyAST, 0) : bodyAST;

    tree = init_ASTnode(AST_GLUE, P_nil, bodyAST, NULL, postopAST, 0);
    tree = init_ASTnode(AST_WHILE, P_nil, condtionalAST, NULL, tree, 0);
    return init_ASTnode(AST_GLUE, P_nil, preopAST, NULL, tree, 0);
}

struct ASTnode *parser_parse_decl(parser_T *parser)
{
    struct token *pk = token_peek(parser->lexer, 2);

    if (pk->token == T_ASSIGN || pk->token == T_SEMI) return parser_parse_variable_decl(parser);

    int type = get_prem_type(eat(parser, parser->token->token)->token);

    char *ident = calloc(1, sizeof(strlen(parser->token->value)));
    strcpy(ident, parser->token->value);
    eat(parser, T_IDENT);

    create_symb_table(ident, type, S_FUNCTION, 0); // 0 to get_label()
    int slot = symb_table_get(ident);
    current_function_call = slot;
    eat(parser, T_LPAREN);
    eat(parser, T_RPAREN);

    struct ASTnode *tree = parser_parse_compound_statements(parser);
    struct ASTnode *last;

    if (type != P_void)
    {
        last = (tree->op == AST_GLUE) ? tree->right : tree;
        if (last == NULL || last->op != AST_RETURN) log(3, "%s", "Non-void returns a valid value.");
    }

    return ASTnode_unary(AST_FUNCTION, type, tree, slot);
}

struct ASTnode *parser_parse_call(parser_T *parser)
{
    struct token *pk = token_peek(parser->lexer, 1);
    if (pk->token == T_ASSIGN) return parser_parse_assignment(parser);

    struct ASTnode *tree;

    int id = symb_table_get(parser->token->value); // -> -1
    
    if (id == -1)
    {
        log(2, "%s", "implicit function call.");
        create_symb_table(parser->token->value, P_i32, S_FUNCTION, 0);

        id = symb_table_get(parser->token->value); // -> -1
    }

    // if (id != -1) { if (symb_table_find(id)->stype != S_FUNCTION) log(3, "Undefined function `%s`.", parser->token->value); }
    // else log(3, "Undefined symbol `%s`.", parser->token->value);

    eat(parser, T_IDENT);

    eat(parser, T_LPAREN);
    tree = parser_parse_expr(parser, 0);
    tree = ASTnode_unary(AST_FUNCTION_CALL, symb_table_find(id)->type, tree, id);
    eat(parser, T_RPAREN);

    return tree;
}

struct ASTnode *parser_parse_return(parser_T *parser)
{
    if (symb_table_find(current_function_call)->type == P_void) log(3, "%s", "void function cannot return.");
    struct ASTnode *tree;

    eat(parser, T_RETURN);
    tree = parser_parse_expr(parser, 0);
    if (tree->op == AST_INTLIT) tree->type = symb_table_find(current_function_call)->type;

    if (tree->type != symb_table_find(current_function_call)->type) log(3, "%s", "function return type don't match.");
    tree = ASTnode_unary(AST_RETURN, P_nil, tree, current_function_call);
    eat(parser, T_SEMI);

    return tree;
}

struct ASTnode *parser_parse_statement(parser_T *parser)
{
    struct ASTnode *tree;

    switch (parser->token->token)
    {
        case T_PRINT: tree = parser_parse_print(parser); break;
        case T_IF: tree = parser_parse_if(parser); break;
        case T_WHILE: tree = parser_parse_while(parser); break;
        case T_FOR: tree = parser_parse_for(parser); break;
        case T_RETURN: tree = parser_parse_return(parser); break;
        case T_byte: tree = parser_parse_decl(parser); break;
        case T_i16: tree = parser_parse_decl(parser); break;
        case T_i32: tree = parser_parse_decl(parser); break;
        case T_i64: tree = parser_parse_decl(parser); break;
        case T_void: tree = parser_parse_decl(parser); break;
        case T_IDENT: tree = parser_parse_call(parser); break;
        default: tree = parser_parse_expr(parser, 0); break;
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
        switch (parser->token->token)
        {
            case T_LBRACE: tree = parser_parse_compound_statements(parser); break;
            default: tree = parser_parse_statement(parser);
        }
        // tree = parser_parse_statement(parser);

        if (tree)
        {
            if (left == NULL) left = tree;
            else left = init_ASTnode(AST_GLUE, P_nil, left, NULL, tree, 0);
        }

        if (parser->token->token == T_SEMI) eat(parser, T_SEMI);
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
            else left = init_ASTnode(AST_GLUE, P_nil, left, NULL, tree, 0);
        }
    }

    left = (left == NULL) ? ASTnode_unary(AST_NOOP, P_nil, left, 0) : left;

    return left;
}

int get_prem_type(int type)
{
    switch (type)
    {
        case T_void: return P_void;
        case T_byte: return P_byte;
        case T_i16:return P_i16;
        case T_i32: return P_i32;
        case T_i64: return P_i64;
        default: log(3, "%s", "Unrecognised data type.");
    }

    return -1;
}

int get_prem_size(int type)
{
    switch(type)
    {
        case P_void: return 0;
        case P_byte: return 1;
        case P_i16: return 2;
        case P_i32: return 4;
        case P_i64: return 8;
        default: return 0;
    }
}

char *get_prem_size_str(int size)
{
    switch (size)
    {
        case 1: return "byte";
        case 2: return "i16";
        case 4: return "i32";
        case 8: return "i64";
        default: return "invalid size.";
    }
}

int type_check(int left, int right)
{
    if (left == right) return 0;

    int left_size = get_prem_size(left);
    int right_size = get_prem_size(right);

    if (left_size == 0 || right_size == 0) return 1;

    if (left_size < right_size) log(2, "`%s` size is smaller than `%s`.", get_prem_size_str(left_size), get_prem_size_str(right_size));
    if (left_size > right_size) log(2, "`%s` size is smaller than `%s`.", get_prem_size_str(right_size), get_prem_size_str(left_size));

    return 0;
}
