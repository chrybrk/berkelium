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
  T_MODULO,
  T_LPAREN,
  T_RPAREN,
  T_LBRACE,
  T_RBRACE,
  T_SEMI,
  T_COMMA,
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
  T_IF,
  T_ELSE,
  T_WHILE,
  T_FOR,
  T_RETURN,

  // data types
  T_void,
  T_i16,
  T_i32,
  T_i64,
  T_byte,

  // stuff
  T_AMPER,
  T_LOGAND
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
struct token *token_peek(lexer_T *lexer, int offset);

void token_print(lexer_T* lexer);
int is_keyword(char *s);
int which_keyword(int loc);
int is_data_type(char *s);
int which_data_type(int loc);
