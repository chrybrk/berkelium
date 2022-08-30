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
  T_LBRACE,
  T_RBRACE,
  T_SEMI,
  T_ASSIGN,
  T_NOT,
  T_EQU,
  T_NEQ,
  T_GT,
  T_LT,
  T_GEQ,
  T_LEQ,
  T_INTLIT,
  T_IDENT,
  T_EOF,

  // keywords
  T_PRINT,
  // data types
  i32,
  i16
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
int is_data_type(char *s);
int which_data_type(int loc);
