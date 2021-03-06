#ifndef COMMON_H
#define COMMON_H

typedef unsigned int   uint32_t;
typedef signed   int   int32_t;
typedef unsigned short uint16_t;
typedef signed   short int16_t;
typedef unsigned char  uint8_t;
typedef signed   char  int8_t;
typedef uint32_t       size_t;

#define CU32(x) (*((uint32_t*)&x))

#define member_offset(struct, member) ((size_t)&(((struct*)0)->member))

#endif
