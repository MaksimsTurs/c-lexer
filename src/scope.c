#include "scope.h"

static t_scope g_scope = {
  .fns = NULL,
  .fns_capacity = 0,
  .fns_length = 0,

  .vars = NULL,
  .vars_capacity = 0,
  .vars_length = 0
};

void scope_analyse()
{
  t_token* token = lexer_next_token();

  while(!lexer_is_end_of_list())
  {
    if(token->type == TOKEN_KEYWORD && !strncmp("fn", token->value, token->value_length))
    {
      scope_add_fn(&g_scope);
    }
    
    token = lexer_next_token();
  }

  lexer_set_index(0);
}

t_fn* scope_add_fn(t_scope* scope)
{
  t_fn* new_fn = scope_consume_fn(scope);
  t_token* token = lexer_next_token();

  new_fn->name = token;

  token = lexer_next_token();

  while(!lexer_is_end_of_list() && token->type != TOKEN_CBRACKET_L)
  {
    if(token->type == TOKEN_IDENTIFIER)
    {
      scope_add_fn_arg(new_fn);
    }

    token = lexer_next_token();
  }

  new_fn->stack_size += new_fn->args_length * 8;

  while(!lexer_is_end_of_list() && token->type != TOKEN_CBRACKET_R)
  {
    if(token->type == TOKEN_KEYWORD && !strncmp("var", token->value, token->value_length))
    {
      scope_add_local_var(new_fn->body, new_fn);
    }
    
    token = lexer_next_token();
  }

  return new_fn;
}

t_arg* scope_add_fn_arg(t_fn* fn)
{
  t_token* token = lexer_peek_token(0);
  t_arg* new_arg = NULL;

  if(token->type == TOKEN_IDENTIFIER)
  {
    new_arg = scope_consume_fn_arg(fn);
    new_arg->name = token;
  }
  
  return new_arg;
}

t_var* scope_add_local_var(t_scope* scope, t_fn* fn)
{
  t_token* token = lexer_next_token();
  t_var* new_var = scope_consume_var(fn->body);

  fn->stack_size += 8;
  
  new_var->name = token;
  new_var->offset = fn->stack_size;
  lexer_next_token();
  new_var->value = lexer_next_token();

  return new_var;
}

t_arg* scope_consume_fn_arg(t_fn* fn)
{
  t_arg* new_arg = NULL;

  if(fn->args_capacity == 0 || scope_should_reserve_memory(fn->args_length, fn->args_capacity))
  {
    fn->args_capacity += SCOPE_DEFAULT_CAPACITY;
    scope_reserve_memory((void**)(&fn->args), fn->args_capacity * sizeof(t_fn));
  }

  new_arg = &fn->args[fn->args_length];

  new_arg->name = NULL;
  new_arg->index = fn->args_length;

  fn->args_length++;

  return new_arg;
}

t_fn* scope_consume_fn(t_scope* scope)
{
  t_fn* new_fn = NULL;

  if(scope->fns_capacity == 0 || scope_should_reserve_memory(scope->fns_length, scope->fns_capacity))
  {
    scope->fns_capacity += SCOPE_DEFAULT_CAPACITY;
    scope_reserve_memory((void**)(&scope->fns), scope->fns_capacity * sizeof(t_fn));
  }

  new_fn = &scope->fns[scope->fns_length++];

  new_fn->name = NULL;
  new_fn->stack_size = 0;

  new_fn->body = (t_scope*)malloc(sizeof(t_scope));
  new_fn->body->parent = NULL;
  new_fn->body->fns = NULL;
  new_fn->body->fns_capacity = 0;
  new_fn->body->fns_length = 0;
  new_fn->body->vars = NULL;
  new_fn->body->vars_capacity = 0;
  new_fn->body->vars_length = 0;

  return new_fn;
}

t_var* scope_consume_var(t_scope* scope)
{
  t_var* new_var = NULL;

  if(scope->vars_capacity == 0 || scope_should_reserve_memory(scope->vars_length, scope->vars_capacity))
  {
    scope->vars_capacity += SCOPE_DEFAULT_CAPACITY;
    scope_reserve_memory(((void**)&scope->vars), scope->vars_capacity * sizeof(t_var));
  }

  new_var = &scope->vars[scope->vars_length++];
  new_var->name = NULL;
  new_var->value = NULL;
  new_var->offset = 0;

  return new_var;
}

void scope_reserve_memory(void** ptr, long long size)
{
  if(*ptr == NULL)
  {
    *ptr = malloc(size);
  }
  else 
  {
    *ptr = realloc(*ptr, size);
  }
}

unsigned char scope_should_reserve_memory(long long length, long long capacity)
{
  return length >= capacity;
}

t_fn* scope_get_fn(t_token* name)
{
  t_fn* fn = NULL;
  long long length = 0;

  for(long long index = 0; index < g_scope.fns_length; index++)
  {
    fn = &g_scope.fns[index];
    length = fn->name->value_length > name->value_length ? fn->name->value_length : name->value_length;

    if(!strncmp(fn->name->value, name->value, length))
    {
      return fn;
    }
  }

  return NULL;
}

t_var* scope_get_var(t_scope* scope, t_token* name)
{
  t_var* var = NULL;
  long long length = 0;

  for(long long index = 0; index < scope->vars_length; index++)
  {
    var = &scope->vars[index];
    length = var->name->value_length > name->value_length ? var->name->value_length : name->value_length;

    if(!strncmp(var->name->value, name->value, length))
    {
      return var;
    }
  }

  if(scope->parent != NULL)
  {
    return scope_get_var(scope->parent, name);
  }

  return NULL;
}

t_scope* scope_get_global()
{
  return &g_scope;
}