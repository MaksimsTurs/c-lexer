#include "utils.h"

unsigned char cmp_strn(const char* f_str, const char* s_str, long count)
{
  while(count--)
  {
    if(f_str[count] != s_str[count])
    {
      return 0;
    }
  }

  return 1;
}