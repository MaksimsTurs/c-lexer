#include "parser.h"

static long long     g_tokens_index = 0;
static t_token*      g_token        = NULL;
static t_token_list* g_tokens_list  = NULL;
static t_stmt        g_programm     = {0};

void parser_init(t_token_list* tokens)
{
  if(tokens == NULL)
  {
    LOG(ERROR, "Tokens list is null!\n");
    exit(-1);
  }

  g_tokens_list = tokens;
}

void parser_parse(t_token_list* tokens)
{
  g_tokens_list = tokens;
  g_programm.type = AST_STMT_PROGRAMM;

  t_token* token = next_token();
  
  print_ast(parser_parse_unar_expr(token));
}

t_expr* parser_parse_primary_expr(t_token* token)
{
  if(token != NULL)
  {
    t_expr* expr = (t_expr*)malloc(sizeof(t_expr));
  
    expr->expr = (void*)token;
    expr->type = token_type_to_ast_type(token->type);
  
    return expr;
  }

  return NULL;
}

t_expr* parser_parse_bin_expr(t_token* token)
{
  if(token != NULL)
  {
    t_bin_expr* bin_expr = (t_bin_expr*)malloc(sizeof(t_bin_expr));
    t_expr* node_expr = (t_expr*)malloc(sizeof(t_expr));

    node_expr->type = AST_EXPR_BINARY;
    node_expr->expr = bin_expr;

    bin_expr->right = parser_parse_primary_expr(token);
    bin_expr->op = next_token();
    bin_expr->left = parser_parse_bin_expr(next_token());

    return node_expr;
  }

  return NULL;
}

t_expr* parser_parse_unar_expr(t_token* token)
{
  if(token != NULL)
  {
    t_unar_expr* unar_expr = (t_unar_expr*)malloc(sizeof(t_unar_expr));
    t_expr* expr = (t_expr*)malloc(sizeof(t_expr));

    expr->type = AST_EXPR_UNARY;
    expr->expr = unar_expr;

    unar_expr->operator = token;
    unar_expr->expr = parser_parse_bin_expr(next_token());

    return expr;
  }

  return NULL;
}

void print_ast(t_expr* root)
{
  static int margin = 0;

  switch(root->type)
  {
    case AST_STMT_PROGRAMM:
      PRINT_WITH_MARGIN(margin, "%*sProgramm:\n");
      margin += 2;

      print_ast(root->expr);
    break;
    case AST_EXPR_UNARY:
    {
      t_unar_expr* unar_expr = (t_unar_expr*)root->expr;
      PRINT_WITH_MARGIN(margin, "%*sUnary:\n");
      margin += 2;
      
      PRINT_WITH_MARGIN(margin, "%*sOp: %s\n", unar_expr->operator->buffer);
      print_ast(unar_expr->expr);
    }
    break;
    case AST_EXPR_BINARY:
    {
      t_bin_expr* bin_expr = (t_bin_expr*)root->expr;
      PRINT_WITH_MARGIN(margin, "%*sBinary:\n");
      margin +=2;

      if(bin_expr->left)
      {
        PRINT_WITH_MARGIN(margin, "%*sLeft:");
        if(bin_expr->left->type != AST_EXPR_NUMBER_LITERAL &&
           bin_expr->left->type != AST_EXPR_IDENTIFIER)
        {
          printf("\n");
        }

        margin +=2;
        print_ast(bin_expr->left);
        margin -= 2;
      }

      if(bin_expr->op)
      {
        PRINT_WITH_MARGIN(margin, "%*sOp: %s\n", bin_expr->op->buffer);
      }

      if(bin_expr->right)
      {
        PRINT_WITH_MARGIN(margin, "%*sRight:");
        if(bin_expr->right->type != AST_EXPR_NUMBER_LITERAL &&
           bin_expr->right->type != AST_EXPR_IDENTIFIER)
        {
          printf("\n");
        }

        margin +=2;
        print_ast(bin_expr->right);
      }
    }
    break;
    case AST_EXPR_IDENTIFIER:
    case AST_EXPR_NUMBER_LITERAL:
    {
      t_token* token = (t_token*)root->expr;
      PRINT_WITH_MARGIN(1, "%*s%s\n", token->buffer);
    }
    break;
  }

  margin -= 2;
}

t_token* peek_token(long long offset)
{
  if(g_tokens_index + offset < g_tokens_list->length)
  {
    return &g_tokens_list->tokens[(g_tokens_index + offset) - 1];
  }

  return NULL;
}

t_token* next_token()
{
  if(g_tokens_index < g_tokens_list->length)
  {
    return &g_tokens_list->tokens[g_tokens_index++];
  }

  return NULL;
}

t_token* prev_token()
{
  if(g_tokens_index > 0)
  {
    g_tokens_index--;
    return &g_tokens_list->tokens[g_tokens_index];
  }

  return NULL;
}

int token_type_to_ast_type(int token_type)
{
  switch(token_type)
  {
    case TOKEN_NUMBER_LITERAL: return AST_EXPR_NUMBER_LITERAL;
    case TOKEN_IDENTIFIER:     return AST_EXPR_IDENTIFIER;
    case TOKEN_NOT:            return AST_EXPR_UNARY;
    default:
      LOG(ERROR, "Unknown token type %i!\n", token_type);
      exit(-1);
  }
}