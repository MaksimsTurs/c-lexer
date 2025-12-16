#include "lexer.h"

static t_cmap keywords = {0};
static t_lexer lexer = {
  .ch            = 0,
  .source        = NULL,
  .tokens        = {0},
  .tokens_length = -1,
  .col           = 0,
  .row           = 0
};

t_lexer* get_lexer()
{
  return &lexer;
}

int init_lexer(const char* entry_point)
{
  t_file* file = fopen(entry_point, "r");

  if(file == NULL)
  {
    fclose(file);
    return errno;
  }

  lexer.source = file;
  lexer.ch = getc(lexer.source);
  lexer.keywords = &keywords;
        
  cmap_dinit(&keywords, 10, 0);

  cmap_set(&keywords, "if",     "if");
  cmap_set(&keywords, "else",   "else");
  cmap_set(&keywords, "for",    "for");
  cmap_set(&keywords, "import", "import");
  cmap_set(&keywords, "fn",     "fn");
  cmap_set(&keywords, "return", "return");

  return 0;
}

char get_char()
{
  return lexer.ch;
}

void read_char()
{
  lexer.ch = getc(lexer.source);
  lexer.col++;
}

unsigned char is_keyword(const char* value)
{
  return cmap_has(&keywords, value);
}

unsigned char is_eof()
{
  return lexer.ch == EOF;  
}

unsigned char is_space()
{
  return lexer.ch == ' ';
}

unsigned char is_alphanumeric()
{
  return(
    (lexer.ch >= 'A' && lexer.ch <= 'Z') ||
    (lexer.ch >= 'a' && lexer.ch <= 'z')
  );
}

unsigned char is_numeric()
{
  return lexer.ch >= '0' && lexer.ch <= '9';
}

char create_token(int token_type)
{
  if(token_type != TOKEN_TYPE_WHITE_SPACE &&
     token_type != TOKEN_TYPE_NEW_LINE &&
     token_type != TOKEN_TYPE_CARRIAGE_RETURN)
  {
    lexer.tokens_length++;
  }

  t_token* token = &lexer.tokens[lexer.tokens_length];

  switch(token_type)
  {
    case TOKEN_TYPE_IDENTIFIER:
      while(is_alphanumeric() || is_numeric())
      {
        token->buffer[token->buffer_length] = get_char();
        token->buffer_length++;

        read_char();

        if((is_alphanumeric() || is_numeric()) && 
           token->buffer_length == (TOKEN_BUFFER_CAPACITY - 2))
        {
          return -1;
        }
      }

      token->buffer_length++;
      token->buffer[token->buffer_length] = '\0';
      
      if(is_keyword(token->buffer))
      {
        token->type = TOKEN_TYPE_KEYWORD;
      }
      else
      {
        token->type = TOKEN_TYPE_IDENTIFIER;
      }
    break;
    case TOKEN_TYPE_STRING_LITERAL:
      token->type = TOKEN_TYPE_STRING_LITERAL;
      token->buffer[token->buffer_length] = '\'';
      token->buffer_length++;

      read_char();

      while(get_char() != '"')
      {
        token->buffer[token->buffer_length] = get_char();
        token->buffer_length++;

        read_char();
  
        if(get_char() != '"' && 
           token->buffer_length == (TOKEN_BUFFER_CAPACITY - 3))
        {
          return -1;
        }
      }

      token->buffer[++token->buffer_length] = '\'';
      token->buffer[++token->buffer_length] = '\0';

      read_char();
    break;
    case TOKEN_TYPE_ASSIGN_OP:
    case TOKEN_TYPE_SMALLER_THAN_OP:
    case TOKEN_TYPE_BIGGER_THAN_OP:
      token->buffer[token->buffer_length++] = get_char();

      read_char();

      if(get_char() == '=')
      {
        token->buffer[token->buffer_length++] = get_char();
        token->buffer[token->buffer_length] = '\0';

        switch(token_type)
        {
          case TOKEN_TYPE_ASSIGN_OP:       token->type = TOKEN_TYPE_EQUAL_TO_OP;
          case TOKEN_TYPE_SMALLER_THAN_OP: token->type = TOKEN_TYPE_SMALLER_EQUAL_THAN_OP;
          case TOKEN_TYPE_BIGGER_THAN_OP:  token->type = TOKEN_TYPE_BIGGER_EQUAL_THAN_OP;
        }
        
        read_char();
      } 
      else
      {
        token->type = token_type;
      }
    break;
    case TOKEN_TYPE_NUMBER_LITERAL:
      while(is_numeric() || get_char() == '.')
      {
        token->buffer[token->buffer_length++] = get_char();
        read_char();

        if((is_numeric() || get_char() == '.') && 
           (token->buffer_length > TOKEN_BUFFER_CAPACITY - 3))
        {
          return -1;
        }
      }

      token->buffer[token->buffer_length] = '\0';
      token->type = TOKEN_TYPE_NUMBER_LITERAL;
    break;
    case TOKEN_TYPE_NEW_LINE:
    case TOKEN_TYPE_WHITE_SPACE:
    case TOKEN_TYPE_CARRIAGE_RETURN:
      if(token_type == TOKEN_TYPE_NEW_LINE)
      {
        lexer.row++;
      }

      while(is_space() || get_char() == '\n' || get_char() == '\r')
      {
        read_char();
      }
    break;
    case TOKEN_TYPE_PARENT_BRACKET_LEFT:
    case TOKEN_TYPE_PARENT_BRACKET_RIGHT:
    case TOKEN_TYPE_CURLY_BRACKET_LEFT:
    case TOKEN_TYPE_CURLY_BRACKET_RIGHT:
    case TOKEN_TYPE_COMMA:
    case TOKEN_TYPE_PLUS_OP:
    case TOKEN_TYPE_MINUS_OP:
    case TOKEN_TYPE_DIV_OP:
    case TOKEN_TYPE_MUL_OP:
      token->buffer[token->buffer_length++] = get_char();
      token->buffer[token->buffer_length] = '\0';
      token->type = token_type;
      
      read_char();
    break;
    case TOKEN_TYPE_UNREACHABLE:
      token->buffer[token->buffer_length++] = get_char();
      token->type = TOKEN_TYPE_UNREACHABLE;

      read_char();
    break;
  }

  return 0;
}

t_token* get_tokens()
{
  return lexer.tokens;
}