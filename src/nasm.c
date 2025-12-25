
#include "nasm.h"
#include "module.h"

void nasm_generate(const char* output_path)
{
  t_module new_module = module_create(output_path);
  t_token* token = lexer_next_token();

  // Collect information about the function (Arguments/Local variables) and global
  // variables.
  while(token)
  {
    if(token->type == TOKEN_KEYWORD && !strcmp(token->buffer, "fn"))
    {
      t_fn* fn = module_consume_fn(&new_module.fn_list);

      module_collect_fn_args(fn);
      module_collect_fn_local_vars(fn);
    }

    token = lexer_next_token();
  }

  lexer_set_list_index(0);
  token = lexer_next_token();

  fprintf(new_module.file, "section .text\n\n");

  for(long long index = 0; index < new_module.fn_list.length; index++)
  {
    t_fn* fn = &new_module.fn_list.items[index];

    fprintf(new_module.file, "global %s\n", fn->name);
  }

  fprintf(new_module.file, "extern exit\n");
  fprintf(new_module.file, "extern putchar\n");
  fprintf(new_module.file, "\n");

  while(token)
  {
    if(token->type == TOKEN_KEYWORD && !strcmp("fn", token->buffer))
    {
      nasm_generate_fn_body(&new_module);
    }

    token = lexer_next_token();
  }

  fclose(new_module.file);
}

void nasm_generate_fn_call(t_module* module, const t_fn* caller)
{
  t_token* token = lexer_expect_and_peek_token(TOKEN_IDENTIFIER, 0);
  const char* calle_name = token->buffer;
  long long register_index = 0;


  if(!strcmp("putchar", token->buffer) || !strcmp("exit", token->buffer))
  {
    token = lexer_expect_and_next_token(TOKEN_PBRACKET_L);
    token = lexer_next_token();
    

    if(token->type == TOKEN_PBRACKET_R)
    {

    }
    else if(token->type == TOKEN_IDENTIFIER)
    {
      while(token->type != TOKEN_PBRACKET_R)
      {
        if(token->type == TOKEN_IDENTIFIER)
        {
          t_var* arg = module_get_var(&caller->local_vars, token->buffer);
    
          if(arg == NULL)
          {
            LOG_NOT_DEFINED(token);
          }

          fprintf(module->file, "mov %s, %s\n", nasm_get_arg_register_name(register_index++), arg->value);
        }

        fprintf(module->file, "call %s\n", calle_name);
        token = lexer_next_token();
      }

      token = lexer_expect_and_next_token(TOKEN_SEMI);
    }
  }
  else
  {
    t_fn* called_fn = module_get_fn_data(&module->fn_list, token->buffer);
  }
}

void nasm_generate_fn_body(t_module* module)
{
  t_token* token = lexer_expect_and_next_token(TOKEN_IDENTIFIER);
  t_fn* fn = module_get_fn_data(&module->fn_list, token->buffer);
  t_var* var = NULL;

  fprintf(module->file, "%s:\n", fn->name);
  fprintf(module->file, "push rbp\n");
  fprintf(module->file, "mov rbp, rsp\n");
  fprintf(module->file, "sub rsp, %lli\n", fn->local_vars.offset);
  
  while(token->type != TOKEN_CBRACKET_L)
  {
    token = lexer_next_token();
  }
  
  while(token && token->type != TOKEN_CBRACKET_R)
  {
    if(token->type == TOKEN_KEYWORD && !strcmp("var", token->buffer))
    {   
      token = lexer_expect_and_next_token(TOKEN_IDENTIFIER);
      var = module_get_var(&fn->local_vars, token->buffer);
      
      fprintf(module->file, "mov [rbp - %lli], %s\n", var->offset, var->value);
    }
    else if(token->type == TOKEN_IDENTIFIER && lexer_peek_token(1)->type == TOKEN_PBRACKET_L)
    {
      nasm_generate_fn_call(module, fn);
    }

    token = lexer_next_token();
  }


  fprintf(module->file, "pop rbp\n");
  fprintf(module->file, "add rsp, %lli\n", fn->local_vars.offset);
}

const char* nasm_get_arg_register_name(long long index)
{
  static const char* arg_register_names[] = {
    "rcx",
    "rdx",
    "r8",
    "r9"
  };

  return arg_register_names[index];
}