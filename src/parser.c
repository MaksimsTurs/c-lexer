#include "parser.h"
#include "utils.h"

static t_parser parser = {
  .lexer = NULL,
  .root  = NULL,
  .index = 0
};

int init_parser(t_lexer* lexer)
{
  parser.lexer = lexer;

  return 1;
}

t_token* next_token()
{
  if(parser.index <= parser.lexer->tokens_length)
  {
    return &parser.lexer->tokens[parser.index++];
  }

  return NULL;
}

t_node* parse_func_def(t_token* token)
{
  t_node* func_def = create_node(AST_NODE_FUNC_DEFINITION);

  init_list(func_def, 1);
  push_in_list(func_def, token);

  token = next_token();

  if(!cmp_strn("(", token->buffer, token->buffer_length))
  {
    return NULL;
  }

  token = next_token();
  func_def->left = parse_func_args(token);
  token = next_token();
  if(!cmp_strn("{", token->buffer, token->buffer_length))
  {
    return NULL;
  }
  
  func_def->right = parse_block(token);

  return func_def;
}

t_node* parse_block(t_token* token)
{
  t_node* block = create_node(AST_NODE_BLOCK);
  
  init_list(block, 5);

  token = next_token();

  switch(token->type)
  {
    case TOKEN_TYPE_KEYWORD:
    break;
  }

  return block;
}

t_node* parse_func_args(t_token* token)
{
  t_node* arg_list = create_node(AST_NODE_FUNC_ARG);

  init_list(arg_list, 5);
  // First argument.
  push_in_list(arg_list, token);

  while((token = next_token()) && (token->type != TOKEN_TYPE_PARENT_BRACKET_RIGHT))
  {
    if(token->type == TOKEN_TYPE_COMMA)
    {
      continue;
    }

    push_in_list(arg_list, token);
  }

  return arg_list;
}

int init_list(t_node* node, int size)
{
  node->body = (t_token**)malloc(sizeof(t_token*) * size);
  node->body_length = 0;
  node->body_size = size;

  return 0;
}

int push_in_list(t_node* node, t_token* token)
{
  if(node->body_length + 1 == node->body_size)
  {
    node->body_size *= 2;
    node->body = (t_token**)realloc(node->body, sizeof(t_token*) * node->body_size);
  }

  node->body[node->body_length++] = token;

  return 0;
}

t_node* create_node(t_ast_node_type expr_type)
{
  t_node* new_node = NULL;

  new_node = (t_node*)malloc(sizeof(t_node));

  if(new_node == NULL)
  {
    return NULL;
  }

  new_node->left = NULL;
  new_node->right = NULL;
  new_node->body = NULL;
  new_node->body_type = expr_type;

  return new_node;
}