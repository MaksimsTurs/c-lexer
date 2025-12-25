#ifndef H_LEXER
#define H_LEXER

#include <errno.h>
#include <stdio.h>
// TODO: Implement custom set.
#include "c_map.h"
#include "macros.h"

#define TOKEN_BUFFER_CAPACITY    (unsigned char)255
#define TOKEN_LIST_INIT_CAPACITY (unsigned char)10

typedef enum e_token_type {
  // Base
  TOKEN_KEYWORD,
  TOKEN_IDENTIFIER,
  TOKEN_TYPENAME,
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
typedef cmap t_cmap;

typedef struct s_token {
  t_token_type  type;
  char          buffer[TOKEN_BUFFER_CAPACITY];
  unsigned char buffer_length;
  long long     col;
  long long     row;
} t_token;

typedef struct s_token_list {
  t_token*  items;
  long long length;
  long long capacity;
  // Used for iterations throught the items.
  long long index;
} t_token_list;

typedef struct s_lexer {
  char      ch;
  long long col;
  long long row;
} t_lexer;

// Token list 
t_token*      lexer_consume_token();
t_token*      lexer_peek_token(long long offset);
t_token*      lexer_next_token();
t_token*      lexer_expect_and_next_token(unsigned int token_type);
t_token*      lexer_expect_and_peek_token(unsigned int token_type, long long offset);
void          lexer_set_list_index(long long index);
long long     lexer_get_list_index();

// Lexer
t_token_list* lexer_tokenize(const char* entry_point);
void          lexer_init(const char* entry_point);
void          lexer_push_token(int token_type);

char          lexer_peek_char();
void          lexer_next_char();

unsigned char lexer_is_eof();
unsigned char lexer_is_alphanumeric();
unsigned char lexer_is_digit();
unsigned char lexer_is_typename(const char* buffer);
unsigned char lexer_is_keyword(const char* buffer);
unsigned char lexer_is_char(char ch);
unsigned char lexer_is_operator(int token_type);

// Lexer utils 
void          lexer_print_tokens();
const char*   lexer_get_token_type_string(int token_type);

#endif