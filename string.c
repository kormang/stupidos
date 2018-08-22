#include "string.h"

void* memset (void* dst, int c, size_t size) {
  char* t = dst;
  while (size--) {
    *t++ = (uint8_t)c;
  }
  return dst;
}

void* memcpy (void* dst, void* src, size_t size) {
  char* t = dst;
  char* s = src;
  while (size--) {
    *t++ = *s++;
  }
  return dst;
}
