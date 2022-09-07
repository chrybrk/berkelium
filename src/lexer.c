#include "include/lexer.h"
#include "include/macro.h"

#include <ctype.h>

static char *keyword[] = { "print", "if", "else", "while", "for" };
static char *data_type[] = { "i32", "void", "byte" };

struct token *init_token(int token, char *value, int ln, int clm)
{
    struct token *new_tok = calloc(1, sizeof(struct token));
    new_tok->token = token;
    new_tok->value = value;
    new_tok->ln = ln;
    new_tok->clm = clm;

    return new_tok;
}

const char *tok_type_to_string(int token)
{
    switch (token)
    {
        case T_PLUS: return "T_PLUS";
        case T_MINUS: return "T_MINUS";
        case T_STAR: return "T_STAR";
        case T_SLASH: return "T_SLASH";
        case T_MODULO: return "T_MODULO";
        case T_LPAREN: return "T_LPAREN";
        case T_RPAREN: return "T_RPAREN";
        case T_LBRACE: return "T_LBRACE";
        case T_RBRACE: return "T_RBRACE";
        case T_SEMI: return "T_SEMI";
        case T_EQU: return "T_EQU";
        case T_ASSIGN:  return "T_ASSIGN";
        case T_NEQ: return "T_NEQ";
        case T_GT: return "T_GT";
        case T_LT: return "T_LT";
        case T_GEQ: return "T_GEQ";
        case T_LEQ: return "T_LEQ";
        case T_INTLIT: return "T_INTLIT";
        case T_IDENT: return "T_IDENT";
        case T_PRINT: return "T_PRINT";
        case T_IF: return "T_IF";
        case T_ELSE: return "T_ELSE";
        case T_WHILE: return "T_WHILE";
        case T_FOR: return "T_FOR";
        case T_EOF: return "T_EOF";
        case T_i32: return "T_i32";
        case T_void: return "T_void";
        case T_byte: return "T_byte";
        default: return "unknown token";
    }
}

char *tok_string(struct token *token)
{
    const char *type_str = tok_type_to_string(token->token);
    const char *template = "%s: %s";

    char *str = calloc(strlen(type_str) + strlen(template) + 8, sizeof(char));
    sprintf(str, template, type_str, token->value);

    return str;
}

lexer_T *init_lexer(char *src)
{
    lexer_T *new_lexer = calloc(1, sizeof(struct LEXER_STRUCT));

    new_lexer->src = src;
    new_lexer->size = strlen(src);
    new_lexer->i = 0;
    new_lexer->c = new_lexer->src[new_lexer->i];
    new_lexer->ln = 1;
    new_lexer->clm = 1;

    return new_lexer;
}

void lexer_advance(lexer_T *lexer)
{
    if (lexer->i < lexer->size && lexer->c != '\0')
    {
        lexer->clm++;
        lexer->i += 1;
        lexer->c = lexer->src[lexer->i];
    }
}

void skip(lexer_T *lexer)
{
    while (lexer->c == 13 || lexer->c == 10 || lexer->c == ' ' || lexer->c == '\n')
    {
        if (lexer->c == '\n')
        {
            lexer->ln++; lexer->clm = 1;
        }

        lexer_advance(lexer);
    }
}

struct token *lex_advance_current(lexer_T *lexer, int token)
{
    char *value = calloc(1, sizeof(char));
    value[0] = lexer->c;
    value[1] = '\0';

    struct token *tok = init_token(token, value, lexer->ln, lexer->clm);
    lexer_advance(lexer);

    return tok;
}

struct token *lex_advance_twos(lexer_T *lexer, char next, int if_its, int else_its)
{
    if (lexer->src[lexer->i + 1] == next)
    {
        char *word = calloc(1, sizeof(char));
        word[0] = lexer->c;
        word[1] = next;
        word[2] = '\0';
        struct token *tok = init_token(if_its, word, lexer->ln, lexer->clm);
        lexer_advance(lexer);
        lexer_advance(lexer);
        
        return tok;
    }
    return lex_advance_current(lexer, else_its);
}

struct token *lexer_parse_int(lexer_T *lexer)
{
    char *word = calloc(1, sizeof(char));
    int ln = lexer->ln; int clm = lexer->clm;

    while(isdigit(lexer->c))
    {
        word = realloc(word, (strlen(word) + 128) * sizeof(char));
        strcat(word, &lexer->c);
        lexer_advance(lexer);
    }

    return init_token(T_INTLIT, word, ln, clm);
}

struct token *lexer_parse_id(lexer_T *lexer)
{
    char *word = calloc(1, sizeof(char));
    int ln = lexer->ln; int clm = lexer->clm;

    while(isalpha(lexer->c) || isdigit(lexer->c) || lexer->c == '_')
    {
        word = realloc(word, (strlen(word) + 128) * sizeof(char));
        strcat(word, &lexer->c);
        lexer_advance(lexer);
    }

    int keyword_loc = is_keyword(word);
    int data_type_loc = is_data_type(word);

    if (keyword_loc >= 0) return init_token(which_keyword(keyword_loc), word, ln, clm);
    if (data_type_loc >= 0) return init_token(which_data_type(data_type_loc), word, ln, clm);

    return init_token(T_IDENT, word, ln, clm);
}

struct token *lexer_next_token(lexer_T *lexer)
{
    while (lexer->c != '\0')
    {
        skip(lexer);

        if (isdigit(lexer->c)) return lexer_parse_int(lexer);
        if (isalpha(lexer->c)) return lexer_parse_id(lexer);

        switch (lexer->c)
        {
            case '+': return lex_advance_current(lexer, T_PLUS);
            case '-': return lex_advance_current(lexer, T_MINUS);
            case '*': return lex_advance_current(lexer, T_STAR);
            case '/': return lex_advance_current(lexer, T_SLASH);
            case '%': return lex_advance_current(lexer, T_MODULO);
            case ';': return lex_advance_current(lexer, T_SEMI);
            case '=': return lex_advance_twos(lexer, '=', T_EQU, T_ASSIGN);
            case '!': return lex_advance_twos(lexer, '=', T_NEQ, T_NOT);
            case '<': return lex_advance_twos(lexer, '=', T_LEQ, T_LT);
            case '>': return lex_advance_twos(lexer, '=', T_GEQ, T_GT);
            case '(': return lex_advance_current(lexer, T_LPAREN);
            case ')': return lex_advance_current(lexer, T_RPAREN);
            case '{': return lex_advance_current(lexer, T_LBRACE);
            case '}': return lex_advance_current(lexer, T_RBRACE);
            case '\0': break;
            default: log(3, "ln:%d:%d\n\tUnrecognised character `%c`", lexer->ln, lexer->clm, lexer->c);
        }
    }

    return init_token(T_EOF, NULL, lexer->ln, lexer->clm - 1);
}

struct token *token_peek(lexer_T *lexer, int offset)
{
    int save_clm = lexer->clm;
    int save_ln = lexer->ln;
    int save_i = lexer->i;
    int save_c = lexer->c;

    struct token *tok;

    for (int i = 0; i < offset; i++)
    {
        tok = lexer_next_token(lexer);
    }

    lexer->clm = save_clm;
    lexer->ln = save_ln;
    lexer->i = save_i;
    lexer->c = save_c;

    return tok;
}

void token_print(lexer_T *lexer)
{
    char old_char = lexer->c;

    struct token *tok = lexer_next_token(lexer);
    while (tok->token != T_EOF)
    {
        log(1, "[LEXER]: %s", tok_string(tok));
        tok = lexer_next_token(lexer);
    }

    lexer->c = old_char;
    lexer->i = 0;
    lexer->ln = 1;
    lexer->clm = 0;
}

int is_keyword(char *s)
{
    int size = sizeof(keyword) / sizeof(keyword[0]);

    for ( int i = 0; i < size; i++ )
    {
        if (!strcmp(s, keyword[i])) return i;
    }

    return -1;
}

int is_data_type(char *s)
{
    int size = sizeof(data_type) / sizeof(data_type[0]);

    for ( int i = 0; i < size; i++ )
    {
        if (!strcmp(s, data_type[i])) return i;
    }

    return -1;
}

int which_keyword(int loc)
{
    switch(loc)
    {
        case 0: return T_PRINT;
        case 1: return T_IF;
        case 2: return T_ELSE;
        case 3: return T_WHILE;
        case 4: return T_FOR;
    }

    return -1;
}

int which_data_type(int loc)
{
    switch(loc)
    {
        case 0: return T_i32;
        case 1: return T_void;
        case 2: return T_byte;
    }

    return -1;
}
