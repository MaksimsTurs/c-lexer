#ifndef H_ASTRO_EXPR
#define H_ASTRO_EXPR

#include "lexer.h"

typedef struct s_expr     t_expr;

typedef enum e_expr_type {
  EXPR_BINARY,
  EXPR_UNARY,
  EXPR_NUMBER_LITERAL
} t_expr_type;

typedef struct s_expr_literal {
  t_token* value;
} t_expr_literal;

// Wich this struct we can represent a unary and binary expressions.
typedef struct s_expr {
  t_expr_type  left_expr_type;
  void*        left;
  
  t_token_type op;

  t_expr_type  right_expr_type;
  void*        right;
} t_expr;

t_expr* expr_parse();
t_expr* expr_create_expr(void* left, t_token_type op, void* right);

#endif
