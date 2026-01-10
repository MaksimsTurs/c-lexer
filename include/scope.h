#ifndef H_ASTRO_SCOPE
#define H_ASTRO_SCOPE

#include "lexer.h"

typedef struct s_scope t_scope;
typedef struct s_fn    t_fn;
typedef struct s_arg   t_arg;
typedef struct s_var   t_var;

#define SCOPE_DEFAULT_CAPACITY (unsigned char)10

typedef enum e_scope_type {
  SCOPE_GLOBAL,
  SCOPE_FN,
  SCOPE_BLOCK
} t_scope_type;

typedef struct s_arg {
  t_token*  name;
  long long index;
} t_arg;

typedef struct s_fn {
  t_token*  name;
  t_scope*  body;
  long long stack_size;

  t_arg*    args;
  long long args_capacity;
  long long args_length;
} t_fn;

typedef struct s_var {
  t_token*  name;
  t_token*  value;
  long long offset;
} t_var;

typedef struct s_scope {
  t_scope* parent;

  t_fn*     fns;
  long long fns_length;
  long long fns_capacity;

  t_var*    vars;
  long long vars_length;
  long long vars_capacity;
} t_scope;

void scope_analyse();
t_scope* scope_get_global();
t_fn* scope_add_fn(t_scope* scope);
t_arg* scope_add_fn_arg(t_fn* fn);
t_var* scope_add_local_var(t_scope* scope, t_fn* fn);

t_fn* scope_consume_fn(t_scope* scope);
t_arg* scope_consume_fn_arg(t_fn* fn);
t_var* scope_consume_var(t_scope* scope);

void scope_reserve_memory(void** ptr, long long size);
unsigned char scope_should_reserve_memory(long long length, long long capacity);

t_fn* scope_get_fn(t_token* name);
t_var* scope_get_var(t_scope* scope, t_token* name);
t_arg* scope_get_arg(t_fn* fn, t_token* name);

#endif