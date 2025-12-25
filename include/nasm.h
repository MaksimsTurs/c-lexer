#ifndef H_IR
#define H_IR

#include "lexer.h"
#include "module.h"

#define IR_DEFAULT_VAR_LIST_CAPACITY (unsigned char)5

void nasm_generate(const char* output_path);
void nasm_generate_fn_body(t_module* module);
void nasm_generate_fn_call(t_module* module, const t_fn* caller);
const char* nasm_get_arg_register_name(long long index);

#endif // H_IR