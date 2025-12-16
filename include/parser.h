#ifndef H_PARSER
#define H_PARSER

#include "lexer.h"

typedef enum e_ast_node_type {
  AST_NODE_PROGRAMM = 0,
  AST_NODE_FUNC_DEFINITION,
  AST_NODE_FUNC_DECLARATION,
  AST_NODE_FUNC_ARG,
  AST_NODE_BLOCK,
  AST_NODE_RETURN
} t_ast_node_type;

typedef struct s_node {
  t_ast_node_type body_type;
  t_token**       body;
  long            body_length;
  long            body_size;
  struct s_node*  left;
  struct s_node*  right;
} t_node;

typedef struct s_parser {
  t_lexer* lexer;
  t_node*  root;
  long     index;
} t_parser;

int init_list(t_node* node, int size);
int push_in_list(t_node* node, t_token* token);

int init_parser(t_lexer* lexer);
t_node* create_node(t_ast_node_type expr_type);

t_token* next_token();

t_node* parse_block(t_token* token);
t_node* parse_func_def(t_token* token);
t_node* parse_func_args(t_token* token);
t_node* parse_expr(t_token* token);

// functions call
// functions definition
// variable definition/value assigning
// expressions

#endif