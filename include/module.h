#ifndef H_MODULE
#define H_MODULE

#include <stdlib.h>

#include "macros.h"
#include "lexer.h"

#define MODULE_DEFAULT_FN_CAPACITY (unsigned char)5
#define MODULE_DEFAULT_FUNCTION_ARGS_CAPACITY (unsigned char)5

typedef struct s_var {
  const char*   value;
  const char*   name;
  const char*   type_name;
  unsigned char size;
  long long     offset;
  // TODO: Kind(mutable, constant)
} t_var;

typedef struct s_var_list {
  t_var*    items;
  long long offset;
  long long length;
  long long capacity;
} t_var_list;

typedef struct s_fn {
  const char* name;
  t_var_list  args;
  t_var_list  local_vars;
} t_fn;

typedef struct s_fn_list {
  t_fn*     items;
  long long capacity;
  long long length;
  long long index;
} t_fn_list;

typedef struct s_module {
  t_file*   file;
  // Function declarations.
  t_fn_list fn_list;
  // TODO: Global variables.
} t_module;

t_module    module_create(const char* module_path);
t_fn*       module_consume_fn(t_fn_list* fn_list);
t_var*      module_consume_var(t_var_list* var_list);
void        module_init_var_list(t_var_list* var_list);
void        module_collect_fn_args(t_fn* fn);
void        module_collect_fn_local_vars(t_fn* fn);

t_var*      module_get_var(t_var_list* var_list, const char* name);
t_fn*       module_get_fn_data(t_fn_list* fn_list, const char* name);

#endif