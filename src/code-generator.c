#include "code-generator.h"

int generate_code(t_code* code, t_lexer* lexer)
{
  (void)code;

  char* buffer = malloc(CODE_BUFFER_SIZE);
  long buffer_length = CODE_BUFFER_SIZE;
  long buffer_in_use = 0;

  for(long index = 0; index < lexer->tokens_length; index++)
  {
    t_token token = lexer->tokens[index];

    printf("%s\n", token.buffer);
  }

  return 1;
}