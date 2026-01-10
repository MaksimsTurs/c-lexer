#ifndef H_LEXER
#define H_LEXER

#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "macros.h"

#define TOKEN_BUFFER_CAPACITY    (unsigned char)255
#define TOKEN_LIST_INIT_CAPACITY (unsigned char)10

typedef enum e_token_type {
  // Base
  TOKEN_KEYWORD,
  TOKEN_IDENTIFIER,
  TOKEN_UNREACHABLE,
  // Punctuation
  TOKEN_WHITE_SPACE,
  TOKEN_NEW_LINE,
  TOKEN_CARRIAGE_RETURN,
  TOKEN_COMMA,
  TOKEN_DOT,
  TOKEN_SEMI,
  TOKEN_COLON,
  // Brackets
  TOKEN_PBRACKET_L,
  TOKEN_PBRACKET_R,
  TOKEN_CBRACKET_L,
  TOKEN_CBRACKET_R,
  // Comments
  TOKEN_SINGLE_LINE_COMMENT,
  // Literals
  TOKEN_NUMBER_LITERAL,
  TOKEN_STRING_LITERAL,
  // Operations
  TOKEN_PLUS,
  TOKEN_MINUS,
  TOKEN_DIV,
  TOKEN_MUL,
  TOKEN_ASSIGN,
  TOKEN_LESS_THAN,
  TOKEN_GREATER_THAN,
  TOKEN_EQUAL_TO,
  TOKEN_LESS_EQUAL_THAN,
  TOKEN_GREATER_EQUAL_THAN,
  TOKEN_NOT_EQUAL,
  TOKEN_AND,
  TOKEN_BIT_AND,
  TOKEN_OR,
  TOKEN_BIT_OR,
  TOKEN_NOT,
} t_token_type;

typedef FILE t_file;

typedef struct s_token {
  t_token_type type;
  char*        value;
  int          value_length;
  long long    col;
  long long    row;
} t_token;

typedef struct s_token_list {
  t_token*  items;
  long long length;
  long long capacity;
  // Used for iterations throught the items.
  long long index;
} t_token_list;

typedef struct s_lexer {
  char*     pos;
  char      ch;
  long long col;
  long long row;
} t_lexer;

// Token list 
t_token*      lexer_create_token();
t_token*      lexer_consume_token();
t_token*      lexer_peek_token(long long offset);
t_token*      lexer_next_token();
long long     lexer_get_index();
void          lexer_set_index(long long index);

unsigned char lexer_is_end_of_list();

// Lexer
void          lexer_tokenize(const char* entry_point);
void          lexer_open_file(const char* path);
void          lexer_push_token(int token_type);

char*         lexer_get_cursor_pos();
char          lexer_peek_char();
void          lexer_next_char();

unsigned char lexer_is_eof();
unsigned char lexer_is_alphanumeric();
unsigned char lexer_is_digit();
unsigned char lexer_is_keyword(const t_token* token);
unsigned char lexer_is_operator(const t_token* token);
unsigned char lexer_is_char(char ch);

// Lexer utils 
void          lexer_print_tokens();
const char*   lexer_get_token_type_string(int token_type);

#endif