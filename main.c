// clang -Wall -Wextra main.c ./src/lexer ./ -o main && ./main.exe

#include "include/lexer.h"
#include "include/parser.h"
#include "include/utils.h"
#include "include/code-generator.h"

int main(void)
{
  t_token* token = NULL;

  init_lexer("./main.astr");
  init_parser(get_lexer());

  while(!is_eof())
  {
    if(is_alphanumeric())
    {
      create_token(TOKEN_TYPE_IDENTIFIER);
    }
    else if(is_numeric())
    {
      create_token(TOKEN_TYPE_NUMBER_LITERAL);
    }
    else if(is_space())
    {
      create_token(TOKEN_TYPE_WHITE_SPACE);
    }
    else if(get_char() == '(')
    {
      create_token(TOKEN_TYPE_PARENT_BRACKET_LEFT);
    }
    else if(get_char() == ')')
    {
      create_token(TOKEN_TYPE_PARENT_BRACKET_RIGHT);
    }
    else if(get_char() == '{')
    {
      create_token(TOKEN_TYPE_CURLY_BRACKET_LEFT);
    }
    else if(get_char() == '}')
    {
      create_token(TOKEN_TYPE_CURLY_BRACKET_RIGHT);
    } 
    else if(get_char() == '\r')
    {
      create_token(TOKEN_TYPE_CARRIAGE_RETURN);
    }
    else if(get_char() == '\n')
    {
      create_token(TOKEN_TYPE_NEW_LINE);
    }
    else if(get_char() == ',')
    {
      create_token(TOKEN_TYPE_COMMA);
    }
    else if(get_char() == '+')
    {
      create_token(TOKEN_TYPE_PLUS_OP);
    }
    else if(get_char() == '-') 
    {
      create_token(TOKEN_TYPE_MINUS_OP);
    }
    else if(get_char() == '*') 
    {
      create_token(TOKEN_TYPE_MUL_OP);
    }
    else if(get_char() == '/') 
    {
      create_token(TOKEN_TYPE_DIV_OP);
    }
    else if(get_char() == '"')
    {
      create_token(TOKEN_TYPE_STRING_LITERAL);
    }
    else if(get_char() == '<')
    {
      create_token(TOKEN_TYPE_SMALLER_THAN_OP);
    }
    else if(get_char() == '>')
    {
      create_token(TOKEN_TYPE_BIGGER_THAN_OP);
    }
    else if(get_char() == '=') 
    {
      create_token(TOKEN_TYPE_ASSIGN_OP);
    }
    else
    {
      create_token(TOKEN_TYPE_UNREACHABLE);
    }
  }

  t_node programm_ast = {
    .body = NULL,
    .left = NULL,
    .right = NULL,
    .body_length = 0,
    .body_type = AST_NODE_PROGRAMM,
  };

  while((token = next_token()))
  {
    t_token* tmp_token = NULL;

    switch(token->type)
    {
      case TOKEN_TYPE_KEYWORD:
        if(cmp_strn("fn", token->buffer, token->buffer_length))
        {
          tmp_token = next_token();
          
          if(tmp_token->type != TOKEN_TYPE_IDENTIFIER)
          {
            return -1;
          }

          parse_func_def(tmp_token);
        }
      break;
    }
  }

  return 1;
}