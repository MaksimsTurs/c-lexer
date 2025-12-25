#ifndef H_PARSER
#define H_PARSER

#include "lexer.h"
#include "macros.h"

typedef enum e_ast_type {
  AST_EXPR_BINARY,
  AST_EXPR_UNARY,
  AST_EXPR_NUMBER_LITERAL,
  AST_EXPR_IDENTIFIER,

  AST_STMT_PROGRAMM,
} t_ast_type;

typedef struct s_expr {
  t_ast_type type;
  void*      expr;
} t_expr;

typedef struct s_unar_expr {
  t_expr*  expr;
  t_token* operator;
} t_unar_expr;

typedef struct s_bin_expr {
  t_expr*  left;
  t_expr*  right;
  t_token* op;
} t_bin_expr;

typedef struct s_stmt {
  t_ast_type type;
  void*      stmts;
  long long  stmts_length;
} t_stmt;


void parser_parse(t_token_list* tokens);

t_expr* parser_parse_primary_expr(t_token* token);
t_expr* parser_parse_bin_expr(t_token* token);
t_expr* parser_parse_unar_expr(t_token* token);

t_token* peek_token(long long offset);
t_token* next_token();
t_token* prev_token();

void print_ast(t_expr* root);
int token_type_to_ast_type(int token_type);

#endif