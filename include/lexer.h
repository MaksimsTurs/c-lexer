#ifndef H_LEXER
#define H_LEXER

#include <errno.h>
#include <stdio.h>

#include "c_map.h"

#define TOKEN_BUFFER_CAPACITY (char)64

typedef enum e_token_type {
  TOKEN_TYPE_KEYWORD = 0,
  TOKEN_TYPE_IDENTIFIER,

  TOKEN_TYPE_WHITE_SPACE,
  TOKEN_TYPE_NEW_LINE,
  TOKEN_TYPE_CARRIAGE_RETURN,
  TOKEN_TYPE_COMMA,
  
  TOKEN_TYPE_PARENT_BRACKET_LEFT,
  TOKEN_TYPE_PARENT_BRACKET_RIGHT,
  TOKEN_TYPE_CURLY_BRACKET_LEFT,
  TOKEN_TYPE_CURLY_BRACKET_RIGHT,
  
  TOKEN_TYPE_NUMBER_LITERAL,
  TOKEN_TYPE_STRING_LITERAL,
  
  TOKEN_TYPE_PLUS_OP,
  TOKEN_TYPE_MINUS_OP,
  TOKEN_TYPE_DIV_OP,
  TOKEN_TYPE_MUL_OP,
  TOKEN_TYPE_ASSIGN_OP,
  TOKEN_TYPE_SMALLER_THAN_OP,
  TOKEN_TYPE_BIGGER_THAN_OP,
  TOKEN_TYPE_EQUAL_TO_OP,
  TOKEN_TYPE_SMALLER_EQUAL_THAN_OP,
  TOKEN_TYPE_BIGGER_EQUAL_THAN_OP,
  TOKEN_TYPE_NOT_EQUAL_OP,
  
  TOKEN_TYPE_UNREACHABLE,
} t_token_type;

typedef FILE t_file;
typedef cmap t_cmap;

typedef struct s_token {
  t_token_type  type;
  char          buffer[TOKEN_BUFFER_CAPACITY];
  unsigned char buffer_length;
} t_token;

typedef struct s_lexer {
  // TODO: Maybe add a array of FILE* and index for current processed source file.
  t_file* source;
  char    ch;
  t_token tokens[1024];
  t_cmap* keywords;
  long    tokens_length;
  long    col;
  long    row;
} t_lexer;

int init_lexer(const char* entry_point);

char get_char();
void read_char();

unsigned char is_eof();
unsigned char is_space();
unsigned char is_alphanumeric();
unsigned char is_numeric();
unsigned char is_keyword(const char* value);

char create_token(int token_type);
t_lexer* get_lexer();

#endif