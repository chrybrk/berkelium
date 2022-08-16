#include "include/lexer.h"
#include "include/macro.h"

#include <ctype.h>

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
        case T_INTLIT: return "T_INTLIT";
        default: return "unknown token";
    }
}

char *tok_string(struct token *token)
{
    const char *type_str = tok_type_to_string(token->token);
    const char *template = "%s: `%s` on ln:%d:%d";

    char *str = calloc(strlen(type_str) + strlen(template) + 8 + (sizeof(int) * 2), sizeof(char));
    sprintf(str, template, type_str, token->value, token->ln, token->clm);

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

struct token *lexer_next_token(lexer_T *lexer)
{
    while (lexer->c != '\0')
    {
        skip(lexer);

        if (isdigit(lexer->c)) return lexer_parse_int(lexer);

        switch (lexer->c)
        {
            case '+': return lex_advance_current(lexer, T_PLUS);
            case '-': return lex_advance_current(lexer, T_MINUS);
            case '*': return lex_advance_current(lexer, T_STAR);
            case '/': return lex_advance_current(lexer, T_SLASH);
            case '\0': break;
            default: log(3, "ln:%d:%d\n\tUnrecognised character `%c`", lexer->ln, lexer->clm, lexer->c);
        }
    }

    return init_token(T_EOF, NULL, lexer->ln, lexer->clm);
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
