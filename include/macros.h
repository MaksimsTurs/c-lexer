#ifndef H_MACROS
#define H_MACROS

#include <stdio.h>

#define ERROR "\e[1;31m[ERROR]:\e[0;37m "
#define WARN  "\e[1;33m[WARN]:\e[0;37m "
#define INFO  "\e[1;34m[INFO]:\e[0;37m "

#define LOG(lvl, format, ...) do { \
  printf(lvl);                      \
  printf(format, ##__VA_ARGS__);   \
} while(0)

#define LOG_NOT_DEFINED(token) do { \
  LOG(     \
    ERROR, \
    "On line %lli:%lli \"%s\" is not defined!\n", \
    token->row,   \
    token->col,   \
    token->buffer \
  );              \
} while(0)        \

#define LOG_REDEFINITION(token) do { \
  LOG( \
    ERROR, \
    "Redefinition of \"%s\" on line %lli:%lli!\n", \
    token->buffer, \
    token->row,   \
    token->col   \
  );\
} while(0)

#define PRINT_TOKEN(token) do { \
  LOG(INFO, "%.*s\n", (token)->value_length, (token)->value); \
} while(0)

#endif