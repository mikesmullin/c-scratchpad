#ifndef STRING_H
#define STRING_H

#include "Arena.h"
#include "Base.h"

// "Fat Pointer"
typedef struct {
  u32 size;
  char* c_str;
} String_t;

#endif  // STRING_H