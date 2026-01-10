#include "lexer.h"

static t_file* g_file = NULL;
static t_token_list g_token_list = {0};
static t_lexer g_lexer = {0};

// 
// Lexer
//

void lexer_open_file(const char* path)
{
  g_file = fopen(path, "r");
  g_lexer.ch = getc(g_file);
  g_lexer.pos = g_file->_base;
  g_token_list.items = (t_token*)malloc(sizeof(t_token) * TOKEN_LIST_INIT_CAPACITY);
  g_token_list.capacity = TOKEN_LIST_INIT_CAPACITY;
}

void lexer_tokenize(const char* entry_point)
{
  lexer_open_file(entry_point);

  while(!lexer_is_eof())
  {
    if(lexer_is_char('_') || lexer_is_alphanumeric())
    {
      lexer_push_token(TOKEN_IDENTIFIER);
    }
    else if(lexer_is_digit())
    {
      lexer_push_token(TOKEN_NUMBER_LITERAL);
    }
    else
    {
      switch(lexer_peek_char())
      {
        case ' ':  lexer_push_token(TOKEN_WHITE_SPACE); break;
        case '|':  lexer_push_token(TOKEN_BIT_OR); break;
        case '&':  lexer_push_token(TOKEN_BIT_AND); break;
        case ')':  lexer_push_token(TOKEN_PBRACKET_R); break;
        case '(':  lexer_push_token(TOKEN_PBRACKET_L); break;
        case '{':  lexer_push_token(TOKEN_CBRACKET_L); break;
        case '}':  lexer_push_token(TOKEN_CBRACKET_R); break;
        case ',':  lexer_push_token(TOKEN_COMMA); break;
        case '+':  lexer_push_token(TOKEN_PLUS); break;
        case '-':  lexer_push_token(TOKEN_MINUS); break;
        case '*':  lexer_push_token(TOKEN_MUL); break;
        case '/':  lexer_push_token(TOKEN_DIV); break;
        case '"':  lexer_push_token(TOKEN_STRING_LITERAL); break;
        case '!':  lexer_push_token(TOKEN_NOT); break;
        case '<':  lexer_push_token(TOKEN_LESS_THAN); break;
        case '>':  lexer_push_token(TOKEN_GREATER_THAN); break;
        case '=':  lexer_push_token(TOKEN_ASSIGN); break;
        case ':':  lexer_push_token(TOKEN_COLON); break;
        case ';':  lexer_push_token(TOKEN_SEMI); break;
        case '\r': lexer_push_token(TOKEN_CARRIAGE_RETURN); break;
        case '\n': lexer_push_token(TOKEN_NEW_LINE); break;
        default:   lexer_push_token(TOKEN_UNREACHABLE); break;
      }
    }
  }
}

void lexer_push_token(int token_type)
{
  t_token* token = NULL;

  switch(token_type)
  {
    case TOKEN_IDENTIFIER:
    {
      token = lexer_create_token();

      while(lexer_is_alphanumeric() || lexer_is_digit() || lexer_is_char('_'))
      {
        token->value_length++;        
        lexer_next_char();
      }

      if(lexer_is_keyword(token))
      {
        token->type = TOKEN_KEYWORD;
      }
      else
      {
        token->type = TOKEN_IDENTIFIER;
      }
    }
    break;
    case TOKEN_NUMBER_LITERAL:
    {
      token = lexer_create_token();
      token->type = token_type;

      while(lexer_is_digit() || lexer_is_char('.'))
      {
        token->value_length++;
        lexer_next_char();
      }
    }
    break;
    case TOKEN_STRING_LITERAL:
    {
      token = lexer_create_token();
      token->type = TOKEN_STRING_LITERAL;

      lexer_next_char();

      while(!lexer_is_char('"'))
      {
        token->value_length++;
        lexer_next_char();
      }

      token->value_length++;
      lexer_next_char();
    }
    break;
    case TOKEN_ASSIGN:
    case TOKEN_LESS_THAN:
    case TOKEN_GREATER_THAN:
    case TOKEN_NOT:
    {
      token = lexer_create_token();
      token->value_length++;

      lexer_next_char();

      if(lexer_is_char('='))
      {
        token->value_length++;

        switch(token_type)
        {
          case TOKEN_NOT:          token->type = TOKEN_NOT_EQUAL;          break;
          case TOKEN_ASSIGN:       token->type = TOKEN_EQUAL_TO;           break;
          case TOKEN_LESS_THAN:    token->type = TOKEN_LESS_EQUAL_THAN;    break;
          case TOKEN_GREATER_THAN: token->type = TOKEN_GREATER_EQUAL_THAN; break;
        }
        
        lexer_next_char();
      } 
      else
      {
        token->type = token_type;
      }
    }
    break;
    case TOKEN_NEW_LINE:
    case TOKEN_WHITE_SPACE:
    case TOKEN_CARRIAGE_RETURN:
    {
      while(lexer_is_char(' ') || lexer_is_char('\n') || lexer_is_char('\r'))
      {
        lexer_next_char();
      }
    }
    break;
    case TOKEN_PBRACKET_L:
    case TOKEN_PBRACKET_R:
    case TOKEN_CBRACKET_L:
    case TOKEN_CBRACKET_R:
    case TOKEN_COMMA:
    case TOKEN_PLUS:
    case TOKEN_MINUS:
    case TOKEN_MUL:
    case TOKEN_SEMI:
    case TOKEN_COLON:
    {
      token = lexer_create_token();
      token->value_length++;
      token->type = token_type;
      
      lexer_next_char();
    }
    break;
    case TOKEN_DIV:
    {
      char* div_pos = lexer_get_cursor_pos();

      lexer_next_char();

      if(lexer_is_char('/'))
      {
        while(lexer_is_char('\n') && lexer_is_char('\r') && !lexer_is_eof())
        {
          lexer_next_char();
        }
      }
      else
      {
        token = lexer_create_token();
        token->value = div_pos;
        token->value_length++;
        token->type = token_type;
        
        lexer_next_char();
      }
    }
    break;
    case TOKEN_BIT_AND:
    case TOKEN_BIT_OR:
    {
      token = lexer_create_token();
      token->value_length++;

      lexer_next_char();

      if(lexer_is_char('|') || lexer_is_char('&'))
      {
        token->value_length++;

        switch(token_type)
        {
          case TOKEN_BIT_AND: token->type = TOKEN_AND; break;
          case TOKEN_BIT_OR:  token->type = TOKEN_OR;  break;
        }
      }
      else
      {
        token->type = token_type;
      }

      lexer_next_char();
    }
    break;
    case TOKEN_UNREACHABLE:
    {
      token = lexer_create_token();
      token->value_length++;
      token->type = TOKEN_UNREACHABLE;

      lexer_next_char();
    }
    break;
  }
}

char* lexer_get_cursor_pos()
{
  return g_lexer.pos;
}

char lexer_peek_char()
{
  return g_lexer.ch;
}

void lexer_next_char()
{
  g_lexer.ch = getc(g_file);
  g_lexer.pos++;

  if(g_lexer.ch != '\n')
  {
    g_lexer.col++;
  }
  else
  {
    g_lexer.col = 1;
    g_lexer.row++;
  }
}

unsigned char lexer_is_operator(const t_token* token)
{
  return(
    token->type == TOKEN_EQUAL_TO ||
    token->type == TOKEN_NOT_EQUAL ||
    token->type == TOKEN_GREATER_THAN ||
    token->type == TOKEN_LESS_THAN ||
    token->type == TOKEN_GREATER_EQUAL_THAN ||
    token->type == TOKEN_LESS_EQUAL_THAN ||
    token->type == TOKEN_PLUS ||
    token->type == TOKEN_MINUS ||
    token->type == TOKEN_DIV ||
    token->type == TOKEN_MUL ||
    token->type == TOKEN_AND ||
    token->type == TOKEN_OR ||
    token->type == TOKEN_BIT_AND ||
    token->type == TOKEN_BIT_OR
  );
}

unsigned char lexer_is_keyword(const t_token* token)
{
  static const char* keywords[9] = {
    "fn", "return", "if", "else", "for", "var", "const", "continue", "break"
  };
  static const char length = sizeof(keywords) / sizeof(keywords[0]);

  for(char index = 0; index < length; index++)
  {
    if(!strncmp(keywords[index], token->value, strlen(keywords[index])))
    {
      return 1;
    }
  }

  return 0;
}

unsigned char lexer_is_char(char ch)
{
  return g_lexer.ch == ch;
}

unsigned char lexer_is_eof()
{
  return g_lexer.ch == EOF;  
}

unsigned char lexer_is_digit()
{
  return g_lexer.ch >= '0' && g_lexer.ch <= '9';
}

unsigned char lexer_is_alphanumeric()
{
  char ch = lexer_peek_char();

  return(
    (ch >= 'A' && ch <= 'Z') ||
    (ch >= 'a' && ch <= 'z')
  );
}

// 
// Lexer utils
// 

const char* lexer_get_token_type_string(int token_type)
{
  switch(token_type)
  {
    case TOKEN_KEYWORD:            return "KEYWORD";
    case TOKEN_IDENTIFIER:         return "IDENTIFIER";
    case TOKEN_WHITE_SPACE:        return "WHITE_SPACE";
    case TOKEN_NEW_LINE:           return "NEW_LINE";
    case TOKEN_CARRIAGE_RETURN:    return "CARRIAGE_RETURN";
    case TOKEN_COMMA:              return "COMMA";
    case TOKEN_PBRACKET_L:         return "PBRACKET_LEFT";
    case TOKEN_PBRACKET_R:         return "PBRACKET_RIGHT";
    case TOKEN_CBRACKET_L:         return "CBRACKET_LEFT";
    case TOKEN_CBRACKET_R:         return "CBRACKET_RIGHT";
    case TOKEN_NUMBER_LITERAL:     return "NUMBER_LITERAL";
    case TOKEN_STRING_LITERAL:     return "STRING_LITERAL";
    case TOKEN_PLUS:               return "PLUS";
    case TOKEN_MINUS:              return "MINUS";
    case TOKEN_DIV:                return "DIV";
    case TOKEN_MUL:                return "MUL";
    case TOKEN_ASSIGN:             return "ASSIGN";
    case TOKEN_LESS_THAN:          return "LESS_THAN";
    case TOKEN_GREATER_THAN:       return "GREATER_THAN";
    case TOKEN_EQUAL_TO:           return "EQUAL_TO";
    case TOKEN_LESS_EQUAL_THAN:    return "LESS_EQUAL_THAN";
    case TOKEN_GREATER_EQUAL_THAN: return "GREATER_EQUAL_THAN";
    case TOKEN_NOT_EQUAL:          return "NOT_EQUAL";
    case TOKEN_NOT:                return "NOT";
    case TOKEN_AND:                return "AND";
    case TOKEN_OR:                 return "OR";
    case TOKEN_BIT_AND:            return "BIT_AND";
    case TOKEN_BIT_OR:             return "BIT_OR";
    case TOKEN_UNREACHABLE:        return "UNREACHABLE";
    case TOKEN_COLON:              return "COLON";
    case TOKEN_DOT:                return "DOT";
    case TOKEN_SEMI:               return "SEMICOLON";
    default:                       return "UNKNOWN";
  }

}

void lexer_print_tokens()
{
  LOG(INFO, "Count of tokens: %lli\n", g_token_list.length);
  for(long long index = 0; index < g_token_list.length; index++)
  {
    const t_token token = g_token_list.items[index];
    LOG(
      INFO, 
      "[%.3lli] %12s \"%.*s\"\n", 
      index + 1,
      lexer_get_token_type_string(token.type), 
      (int)token.value_length,
      token.value
    );
  }
}

// 
// Tokens list
// 

void lexer_set_list_index(long long index)
{
  g_token_list.index = index;
}

long long lexer_get_list_index()
{
  return g_token_list.index;
}

t_token* lexer_expect_and_next_token(unsigned int token_type)
{
  t_token* token = lexer_next_token();

  return token;
}

t_token* lexer_expect_and_peek_token(unsigned int token_type, long long offset)
{
  t_token* token = &g_token_list.items[(g_token_list.index - 1) + offset];

  return token;
}

t_token* lexer_consume_token()
{
  if(g_token_list.length + 1 > g_token_list.capacity)
  {
    g_token_list.capacity *= 2;
    t_token* tokens = (t_token*)realloc(g_token_list.items, sizeof(t_token) * g_token_list.capacity);

    if(tokens != g_token_list.items)
    {
      g_token_list.items = tokens;
    }
  }

  return &g_token_list.items[g_token_list.length++];
}

t_token* lexer_peek_token(long long offset)
{
  long long index = (g_token_list.index - 1) + offset;

  if(g_token_list.length == 0 || 
     index >= g_token_list.length)
  {
    return NULL;
  }

  return &g_token_list.items[index];
}

t_token* lexer_next_token()
{
  t_token* token = NULL;

  if(g_token_list.index == g_token_list.length)
  {
    return NULL;
  }

  token = &g_token_list.items[g_token_list.index++];
  
  return token;
}

t_token* lexer_create_token()
{
  t_token* token = lexer_consume_token();
  
  token->col = g_lexer.col;
  token->row = g_lexer.row;
  token->value = g_lexer.pos;
  token->value_length = 0;

  return token;
}

long long lexer_get_index()
{
  return g_token_list.index;
}

void lexer_set_index(long long index)
{
  g_token_list.index = index;
}

unsigned char lexer_is_end_of_list()
{
  return g_token_list.index >= g_token_list.length;
}