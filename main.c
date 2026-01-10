#include "macros.h"
#include "lexer.h"
#include "nasm.h"
#include "scope.h"
#include "expr.h"

int main(int argc, char** argv)
{
  if(argc != 2)
  {
    LOG(ERROR, "Entry point was not provided!\n");
    exit(-1);
  }

  lexer_tokenize(argv[1]);
  expr_parse();
  // scope_analyse();
  // nasm_generate("output.nasm");

  system("nasm -f win64 output.nasm");

  return 0;
}
