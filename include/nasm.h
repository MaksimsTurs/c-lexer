#ifndef H_IR
#define H_IR

#include "lexer.h"
#include "scope.h"

#define IR_DEFAULT_VAR_LIST_CAPACITY (unsigned char)5

void nasm_generate(const char* output_path);
void nasm_generate_fn_body_prologue(t_file* file, t_fn* fn);
void nasm_generate_fn_body_epilogue(t_file* file, t_fn* fn);
void nasm_generate_fn_var_declarations(t_file* file, t_fn* fn);
void nasm_generate_fn_call(t_file* file, t_fn* caller, t_fn* callee);
void nasm_generate_fn_body(t_file* file);
void nasm_generate_fn_declarations(t_file* file, t_scope* scope);

const char* nasm_get_register_name(long long index);

#endif // H_IR