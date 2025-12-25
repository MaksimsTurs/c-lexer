#include "lexer.h"

static t_cmap g_keywords = {0};
static t_cmap g_typenames = {0};
static t_file* g_file = NULL;
static t_token_list g_token_list = {
  .capacity = 0,
  .length = 0,
  .items = NULL
};
static t_lexer g_lexer = {
  .ch = 0,
  .col = 1,
  .row = 1
};

// 
// Lexer
//

void lexer_init(const char* entry_point)
{
  g_file = fopen(entry_point, "r");

  if(g_file == NULL)
  {
    LOG(ERROR, "Can not open file \"%s\"\n!", entry_point);
    exit(errno);
  }

  g_lexer.ch = getc(g_file);
  g_token_list.items = (t_token*)malloc(sizeof(t_token) * TOKEN_LIST_INIT_CAPACITY);
  g_token_list.capacity = TOKEN_LIST_INIT_CAPACITY;

  if(g_token_list.items == NULL)
  {
    LOG(ERROR, "Can not initialize a memory for tokens\n!");
    exit(errno);
  }

  cmap_dinit(&g_keywords, 8, 0);

  cmap_set(&g_keywords, "if",     "if");
  cmap_set(&g_keywords, "else",   "else");
  cmap_set(&g_keywords, "for",    "for");
  cmap_set(&g_keywords, "fn",     "fn");
  cmap_set(&g_keywords, "return", "return");
  cmap_set(&g_keywords, "var",    "var");
  cmap_set(&g_keywords, "const",  "const");

  cmap_dinit(&g_typenames, 12, 0);

  cmap_set(&g_typenames, "i8",  "i8");
}

t_token_list* lexer_tokenize(const char* entry_point)
{
  lexer_init(entry_point);

  while(!lexer_is_eof())
  {
    if(lexer_is_alphanumeric())
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

  return &g_token_list;
}

void lexer_push_token(int token_type)
{
  t_token* token = NULL;

  // We skip all this tokens, therefore we don't need to use memory for this
  // tokens.
  if(token_type != TOKEN_WHITE_SPACE &&
     token_type != TOKEN_NEW_LINE &&
     token_type != TOKEN_CARRIAGE_RETURN &&
     token_type != TOKEN_DIV)
  {
    token = lexer_consume_token();
    token->col = g_lexer.col;
    token->row = g_lexer.row;
  }

  // printf("%p %s\n", token, lexer_get_token_type_string(token_type));

  switch(token_type)
  {
    case TOKEN_IDENTIFIER:
    {
      while(lexer_is_alphanumeric() || lexer_is_digit())
      {
        token->buffer[token->buffer_length++] = lexer_peek_char();

        lexer_next_char();

        if((lexer_is_alphanumeric() || lexer_is_digit()) && 
          token->buffer_length == (TOKEN_BUFFER_CAPACITY - 2))
        {
          LOG(
            ERROR, 
            "To long identifier (Max. buffer size %i) on line %lli:%lli \"%s\"", 
            TOKEN_BUFFER_CAPACITY, 
            g_lexer.row, g_lexer.col,
            token->buffer
          );
          exit(-1);
        }
      }

      token->buffer[token->buffer_length++] = '\0';

      if(lexer_is_keyword(token->buffer))
      {
        token->type = TOKEN_KEYWORD;
      }
      else if(lexer_is_typename(token->buffer))
      {
        token->type = TOKEN_TYPENAME;
      }
      else
      {
        token->type = TOKEN_IDENTIFIER;
      }
    }
    break;
    case TOKEN_NUMBER_LITERAL:
    {
      unsigned char dots_count = 0;

      while(lexer_is_digit() || lexer_is_char('.'))
      {
        token->buffer[token->buffer_length++] = lexer_peek_char();

        lexer_next_char();

        if((lexer_is_digit() || lexer_is_char('.')) && 
           (token->buffer_length > TOKEN_BUFFER_CAPACITY - 2))
        {
          LOG(
            ERROR, 
            "To long number (Max. buffer size %i) on %lli:%lli \"%s\"!", 
            TOKEN_BUFFER_CAPACITY, 
            g_lexer.row, 
            g_lexer.col, 
            token->buffer
          );
          exit(-1);
        }

        if(lexer_is_char('.'))
        {
          dots_count++;
        }

        if(dots_count == 2)
        {
          break;
        }
      }

      token->buffer[token->buffer_length] = '\0';
      token->type = TOKEN_NUMBER_LITERAL;
    }
    break;
    case TOKEN_STRING_LITERAL:
    {
      token->type = TOKEN_STRING_LITERAL;

      lexer_next_char();

      while(!lexer_is_char('"'))
      {
        token->buffer[token->buffer_length] = lexer_peek_char();
        token->buffer_length++;

        lexer_next_char();
  
        if(!lexer_is_char('"') && token->buffer_length == (TOKEN_BUFFER_CAPACITY - 3))
        {
          LOG(
            ERROR, 
            "To long string (Max. buffer size %i) on %lli:%lli \"%s\"", 
            TOKEN_BUFFER_CAPACITY, 
            g_lexer.row, 
            g_lexer.col, 
            token->buffer
          );
          exit(-1);
        }
      }

      token->buffer[++token->buffer_length] = '\0';

      lexer_next_char();
    }
    break;
    case TOKEN_ASSIGN:
    case TOKEN_LESS_THAN:
    case TOKEN_GREATER_THAN:
    case TOKEN_NOT:
    {
      token->buffer[token->buffer_length++] = lexer_peek_char();

      lexer_next_char();

      if(lexer_is_char('='))
      {
        token->buffer[token->buffer_length++] = lexer_peek_char();
        token->buffer[token->buffer_length] = '\0';

        switch(token_type)
        {
          case TOKEN_NOT:          token->type = TOKEN_NOT_EQUAL; break;
          case TOKEN_ASSIGN:       token->type = TOKEN_EQUAL_TO; break;
          case TOKEN_LESS_THAN:    token->type = TOKEN_LESS_EQUAL_THAN; break;
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
      token->buffer[token->buffer_length++] = lexer_peek_char(); 
      token->buffer[token->buffer_length] = '\0';
      token->type = token_type;
      
      lexer_next_char();
    }
    break;
    case TOKEN_DIV:
      const char ch = lexer_peek_char();

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
        token->buffer[token->buffer_length++] = ch; 
        token->buffer[token->buffer_length] = '\0';
        token->type = token_type;
        
        lexer_next_char();
      }
    break;
    case TOKEN_BIT_AND:
    case TOKEN_BIT_OR:
    {
      token->buffer[token->buffer_length++] = lexer_peek_char();

      lexer_next_char();

      if(lexer_is_char('|') || lexer_is_char('&'))
      {
        token->buffer[token->buffer_length++] = lexer_peek_char();
        token->buffer[token->buffer_length] = '\0';

        switch(token_type)
        {
          case TOKEN_BIT_AND: token->type = TOKEN_AND;
          case TOKEN_BIT_OR:  token->type = TOKEN_OR;
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
      LOG(
        WARN, 
        "Unreachable character \"%c\" in %lli:%lli near \"%s\"!\n", 
        lexer_peek_char(), 
        g_lexer.row, 
        g_lexer.col, 
        lexer_peek_token(-1)->buffer
      );

      token->buffer[token->buffer_length++] = lexer_peek_char();
      token->type = TOKEN_UNREACHABLE;

      lexer_next_char();
    }
    break;
  }
}

char lexer_peek_char()
{
  return g_lexer.ch;
}

void lexer_next_char()
{
  g_lexer.ch = getc(g_file);

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

unsigned char lexer_is_operator(int token_type)
{
  return(
    token_type == TOKEN_EQUAL_TO ||
    token_type == TOKEN_NOT_EQUAL ||
    token_type == TOKEN_GREATER_THAN ||
    token_type == TOKEN_LESS_THAN ||
    token_type == TOKEN_GREATER_EQUAL_THAN ||
    token_type == TOKEN_LESS_EQUAL_THAN ||
    token_type == TOKEN_PLUS ||
    token_type == TOKEN_MINUS ||
    token_type == TOKEN_DIV ||
    token_type == TOKEN_MUL ||
    token_type == TOKEN_AND ||
    token_type == TOKEN_OR ||
    token_type == TOKEN_BIT_AND ||
    token_type == TOKEN_BIT_OR
  );
}

unsigned char lexer_is_keyword(const char* buffer)
{
  return cmap_has(&g_keywords, buffer);
}

unsigned char lexer_is_typename(const char* buffer)
{
  return cmap_has(&g_typenames, buffer);
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
    case TOKEN_TYPENAME:           return "TYPENAME";
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
      "[%.3lli] %12s \"%s\"\n", 
      index, 
      lexer_get_token_type_string(token.type), 
      token.buffer
    );
  }
}

// 
// Tokens list
// 

void lexer_set_list_index(long long index)
{
  if(index > g_token_list.length)
  {
    LOG(ERROR, "Index %lli is bigger then length of the token list!\n", index);
    exit(-1);
  }

  g_token_list.index = index;
}

long long lexer_get_list_index()
{
  return g_token_list.index;
}

t_token* lexer_expect_and_next_token(unsigned int token_type)
{
  t_token* token = lexer_next_token();

  if(token->type != token_type)
  {
    LOG(
      ERROR, 
      "Unexpected token \"%s\" on line %lli:%lli, expect %s!\n", 
      token->buffer, 
      token->row,
      token->col, 
      lexer_get_token_type_string(token_type)
    );
    exit(-1);
  }

  return token;
}

t_token* lexer_expect_and_peek_token(unsigned int token_type, long long offset)
{
  t_token* token = &g_token_list.items[(g_token_list.index - 1) + offset];

  if(token->type != token_type)
  {
    LOG(
      ERROR, 
      "Unexpected token \"%s\" on line %lli:%lli, expect %s!\n", 
      token->buffer, 
      token->row,
      token->col, 
      lexer_get_token_type_string(token_type)
    );
    exit(-1);
  }

  return token;
}

t_token* lexer_consume_token()
{
  if(g_token_list.length + 1 > g_token_list.capacity)
  {
    g_token_list.capacity *= 2;
    t_token* tokens = (t_token*)realloc(g_token_list.items, sizeof(t_token) * g_token_list.capacity);

    if(tokens == NULL)
    {
      LOG(ERROR, "Can not allocate memory for new tokens!\n");
      exit(-1);
    }

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
  if(g_token_list.index == g_token_list.length)
  {
    return NULL;
  }
  
  return &g_token_list.items[g_token_list.index++];
}