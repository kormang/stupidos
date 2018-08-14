#ifndef PIC_H
#define PIC_H
#include "common.h"

#define PIC1_COM 0x20
#define PIC1_DATA 0x21
#define PIC2_COM 0xA0
#define PIC2_DATA 0xA1

#define PIC1_OFFSET_VECTOR 0x20
#define PIC2_OFFSET_VECTOR 0x28

#define IRQ(n) (PIC1_OFFSET_VECTOR + n)

void pic_remap();

#endif
