// clang -Wall -Wextra main.c ./src/lexer ./ -o main && ./main.exe

#include "macros.h"
#include "lexer.h"
#include "parser.h"
#include "nasm.h"
// #include "code-generator.h"

int main(int argc, char** argv)
{
  if(argc != 2)
  {
    LOG(ERROR, "Entry point was not provided!\n");
    exit(-1);
  }

  t_token_list* token_list = lexer_tokenize(argv[1]);
  // lexer_print_tokens();
  nasm_generate("output.nasm");

  // Compile NASM
  // nasm -f win64 output.nasm && clang output.obj -o main.exe && ./main.exe
  system("nasm -f win64 output.nasm");
  
  return 0;
}