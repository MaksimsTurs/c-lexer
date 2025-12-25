#include "module.h"

t_module module_create(const char* module_path)
{
  t_file* file = fopen(module_path, "w+");
  t_module new_module = {0};
  
  new_module.file = file;
  new_module.fn_list.capacity = MODULE_DEFAULT_FN_CAPACITY;
  new_module.fn_list.index = 0;
  new_module.fn_list.length = 0;

  new_module.fn_list.items = (t_fn*)malloc(sizeof(t_fn) * new_module.fn_list.capacity);

  return new_module;
}

t_fn* module_consume_fn(t_fn_list* fn_list)
{
  if(fn_list->length == fn_list->capacity)
  {
    fn_list->capacity *= 2;
    t_fn* fns = (t_fn*)realloc(fn_list->items, fn_list->capacity * sizeof(t_fn));

    if(fns == NULL)
    {
      LOG(ERROR, "Can not allocate memory for functions declarations!\n");
      exit(-1);
    }

    if(fn_list->items != fns)
    {
      fn_list->items = fns;
    }
  }

  return &fn_list->items[fn_list->length++];
}

t_var* module_consume_var(t_var_list* var_list)
{
  if(var_list->length == var_list->capacity)
  {
    var_list->capacity *= 2;
    t_var* vars = (t_var*)realloc(var_list->items, var_list->capacity * sizeof(t_fn));

    if(vars == NULL)
    {
      LOG(ERROR, "Can not allocate memory for variables declarations!\n");
      exit(-1);
    }

    var_list->items = vars;
  }

  return &var_list->items[var_list->length++];
}

void module_init_var_list(t_var_list* var_list)
{
  var_list->capacity = MODULE_DEFAULT_FUNCTION_ARGS_CAPACITY;
  var_list->length = 0;
  var_list->offset = 0;
  var_list->items = (t_var*)malloc(sizeof(t_var) * var_list->capacity);
}

void module_collect_fn_args(t_fn* fn)
{
  t_token* token = lexer_expect_and_next_token(TOKEN_IDENTIFIER);

  fn->name = token->buffer;
  token = lexer_expect_and_next_token(TOKEN_PBRACKET_L);
  token = lexer_next_token();

  if(token->type != TOKEN_PBRACKET_R &&
     token->type != TOKEN_IDENTIFIER)
  {
    LOG(ERROR, "Unexpected token %s!\n", lexer_get_token_type_string(token->type));
    exit(-1);
  }
  
  // Arguments are provided.
  if(token->type == TOKEN_IDENTIFIER)
  {
    module_init_var_list(&fn->args);

    while(token && token->type != TOKEN_PBRACKET_R)
    {
      if(token->type == TOKEN_IDENTIFIER)
      {
        t_var* arg = module_consume_var(&fn->args);
        
        arg->name = token->buffer;
        arg->size = 8;
        arg->type_name = NULL;
        arg->value = NULL;
        fn->args.offset += arg->size;
        arg->offset = fn->args.offset;
      }
      token = lexer_next_token();
    }
  }
}

void module_collect_fn_local_vars(t_fn* fn)
{
  t_token* token = lexer_peek_token(0);

  module_init_var_list(&fn->local_vars);

  while(token && token->type != TOKEN_CBRACKET_R)
  {
    if(token->type == TOKEN_KEYWORD && !strcmp("var", token->buffer))
    {
      t_var* var = NULL;

      token = lexer_expect_and_next_token(TOKEN_IDENTIFIER);
      
      var = module_consume_var(&fn->local_vars);
      var->name = token->buffer;
      var->size = 8;
      var->type_name = NULL;
      
      token = lexer_expect_and_next_token(TOKEN_ASSIGN);
      token = lexer_next_token();
      
      var->value = token->buffer;

      fn->local_vars.offset += var->size;
      var->offset = fn->local_vars.offset;
    }

    token = lexer_next_token();
  }
}

t_var* module_get_var(t_var_list* var_list, const char* name)
{
  for(long long index = 0; index < var_list->length; index++)
  {
    if(!strcmp(name, var_list->items[index].name))
    {
      return &var_list->items[index];
    }
  }

  return NULL;
}

t_fn* module_get_fn_data(t_fn_list* fn_list, const char* name)
{
  for(long long index = 0; index < fn_list->length; index++)
  {
    t_fn* fn = &fn_list->items[index];
    
    if(!strcmp(fn->name, name))
    {
      return fn;
    }
  }

  return NULL;
}