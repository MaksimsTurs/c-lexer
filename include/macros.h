#ifndef H_MACROS
#define H_MACROS

#include <stdio.h>

#define ERROR "\e[1;31m[ERROR]:\e[0;37m "
#define WARN  "\e[1;33m[WARN]:\e[0;37m "
#define INFO  "\e[1;34m[INFO]:\e[0;37m "

#define LOG(lvl, message, ...) do { \
  printf(lvl);                      \
  printf(message, ##__VA_ARGS__);   \
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

#define PRINT_WITH_MARGIN(margin, message, ...)  do { \
  printf(message, margin, "", ##__VA_ARGS__);\
} while(0)

#endif