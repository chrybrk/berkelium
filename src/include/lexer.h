#pragma once

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

enum
{
  T_PLUS,
  T_MINUS,
  T_STAR,
  T_SLASH,
  T_LPAREN,
  T_RPAREN,
  T_SEMI,
  T_INTLIT,
  T_PRINT,
  T_EOF
};

struct token
{
    int token;
    char *value;
    int ln;
    int clm;
};

typedef struct LEXER_STRUCT
{
    char *src;
    size_t size;
    char c;
    unsigned int i;
    int ln;
    int clm;
} lexer_T;

struct token *init_token(int token, char *value, int ln, int clm);
const char *tok_type_to_string(int token);
char *tok_string(struct token *token);

lexer_T *init_lexer(char *src);
void lexer_advance(lexer_T *lexer);
struct token *lexer_next_token(lexer_T *lexer);

void token_print(lexer_T* lexer);
int is_keyword(char *s);
int which_keyword(int loc);
