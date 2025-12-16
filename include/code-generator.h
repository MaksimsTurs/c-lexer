#ifndef H_CODE_GENERATOR
#define H_CODE_GENERATOR

#include "lexer.h"

#define CODE_BUFFER_SIZE 64

typedef struct s_code {
  char* buffer;
  long length;
} t_code;

int generate_code(t_code* code, t_lexer* lexer);

#endif