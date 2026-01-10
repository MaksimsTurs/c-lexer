#include "expr.h"

t_expr* expr_parse()
{
  t_token* token = lexer_next_token();
}

t_expr* expr_create_expr(void* left, t_token_type op, void* right)
{
  t_expr* expr = (t_expr*)malloc(sizeof(t_expr));
  
  expr->left = left;
  expr->op = op;
  expr->right = right;

  return expr;
}