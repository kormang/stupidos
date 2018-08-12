#include "string.h"

void* memset (void* dst, int c, size_t size) {
  char* t = dst;
  while (size--) {
    *t++ = (uint8_t)c;
  }
  return dst;
}
