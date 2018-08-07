#ifndef COMMON_H
#define COMMON_H

typedef unsigned int   uint32_t;
typedef signed   int   int32_t;
typedef unsigned short uint16_t;
typedef signed   short int16_t;
typedef unsigned char  uint8_t;
typedef signed   char  int8_t;
typedef uint32_t       size_t;


void outb(uint16_t port, uint8_t value);
uint8_t inb(uint16_t port);
uint16_t inw(uint16_t port);

#endif
