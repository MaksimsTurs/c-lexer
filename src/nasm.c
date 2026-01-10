#include "nasm.h"

void nasm_generate(const char* output_path)
{
  t_file* file = fopen(output_path, "w+");
  t_token* token = lexer_next_token();

  nasm_generate_fn_declarations(file, scope_get_global());

  while(!lexer_is_end_of_list())
  {
    if(token->type == TOKEN_KEYWORD && !strncmp("fn", token->value, token->value_length))
    {
      nasm_generate_fn_body(file);
    }
    
    token = lexer_next_token();
  }

  fclose(file);
}

void nasm_generate_fn_body(t_file* file)
{
  t_token* token = lexer_next_token(); 
  t_fn* fn = scope_get_fn(token);
  t_fn* callee = NULL;

  nasm_generate_fn_body_prologue(file, fn);
  nasm_generate_fn_var_declarations(file, fn);

  while(token->type != TOKEN_CBRACKET_L)
  {
    token = lexer_next_token();
  }

  while(token->type != TOKEN_CBRACKET_R && !lexer_is_end_of_list())
  {
    if(token->type == TOKEN_IDENTIFIER && lexer_peek_token(1)->type == TOKEN_PBRACKET_L)
    {
      callee = scope_get_fn(token);
      nasm_generate_fn_call(file, fn, callee);
    }

    token = lexer_next_token();
  }

  nasm_generate_fn_body_epilogue(file, fn);
}

void nasm_generate_fn_var_declarations(t_file* file, t_fn* fn)
{
  for(long long index = 0; index < fn->body->vars_length; index++)
  {
    t_var* var = &fn->body->vars[index];

    fprintf(file, "mov [rbp-%lli], %.*s\n", var->offset, var->value->value_length, var->value->value);
  }
}

void nasm_generate_fn_body_prologue(t_file* file, t_fn* fn)
{
  fprintf(file, "%.*s:\n", fn->name->value_length, fn->name->value);
  fprintf(file, "push rbp\n");
  fprintf(file, "mov rbp, rsp\n");

  if(fn->stack_size > 0)
  {
    fprintf(file, "sub rsp, %lli\n", fn->stack_size);
  }

  if(fn->args_length > 0)
  {
    for(long long index = 0; index < fn->args_length; index++)
    {
      t_arg* arg = &fn->args[index];
      fprintf(file, "mov %s, [rbp-%lli]\n", nasm_get_register_name(index), (arg->index + 1) * 8);
    }
  }
}

void nasm_generate_fn_body_epilogue(t_file* file, t_fn* fn)
{
  if(fn->stack_size > 0)
  {
    fprintf(file, "add rsp, %lli\n", fn->stack_size);
  }

  fprintf(file, "pop rbp\n");
  fprintf(file, "ret\n\n");
}

void nasm_generate_fn_declarations(t_file* file, t_scope* scope)
{
  fprintf(file, "section .text\n");
  for(long long index = 0; index < scope->fns_length; index++)
  {
    t_fn* fn = &scope->fns[index];
    fprintf(file, "global %.*s\n", fn->name->value_length, fn->name->value);
  }
  fprintf(file, "\n");
}

void nasm_generate_fn_call(t_file* file, t_fn* caller, t_fn* callee)
{
  t_token* token = lexer_next_token();
  t_var* var = NULL;
  long long index = 0;

  while(token->type != TOKEN_PBRACKET_R)
  {
    if(token->type == TOKEN_IDENTIFIER)
    {
      var = scope_get_var(caller->body, token);
      fprintf(file, "mov %s, [rbp-%lli]\n", nasm_get_register_name(index), var->offset);
      index++;
    }
    else if(token->type == TOKEN_NUMBER_LITERAL)
    {
      fprintf(file, "mov %s, %.*s\n", nasm_get_register_name(index), token->value_length, token->value);
      index++;
    }

    token = lexer_next_token();
  }
  
  fprintf(file, "call %.*s\n", callee->name->value_length, callee->name->value);
}

const char* nasm_get_register_name(long long index)
{
  const static char* registers[4] = {
    "rcx\0", "rdx\0", "r8\0", "r9\0"
  };
  
  return registers[index];
}