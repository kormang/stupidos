#include "pic.h"
#include "io.h"

/* stolen initialization word constants (nice to have it documented here): */
#define ICW1_ICW4	0x01		/* ICW4 (not) needed */
#define ICW1_SINGLE	0x02		/* Single (cascade) mode */
#define ICW1_INTERVAL4	0x04		/* Call address interval 4 (8) */
#define ICW1_LEVEL	0x08		/* Level triggered (edge) mode */
#define ICW1_INIT	0x10		/* Initialization - required! */

#define ICW4_8086	0x01		/* 8086/88 (MCS-80/85) mode */
#define ICW4_AUTO	0x02		/* Auto (normal) EOI */
#define ICW4_BUF_SLAVE	0x08		/* Buffered mode/slave */
#define ICW4_BUF_MASTER	0x0C		/* Buffered mode/master */
#define ICW4_SFNM	0x10		/* Special fully nested (not) */

/*
  Remaps PIC to use different offset vectors,
  because initial offset 0h-7h for PIC1 conflicts with CPU faults and exceptions.
*/
void pic_remap() {
  outb(PIC1_COM, ICW1_INIT+ICW1_ICW4); // start init
  io_wait();
  outb(PIC2_COM,  ICW1_INIT+ICW1_ICW4);
  io_wait();
  outb(PIC1_DATA, PIC1_OFFSET_VECTOR);
  io_wait();
  outb(PIC1_DATA, PIC2_OFFSET_VECTOR);
  io_wait();

  // connect master and slave (slave at IRQ2 of master)
  outb(PIC1_DATA, 4);
  io_wait();
  outb(PIC2_DATA, 2);
  io_wait();

  outb(PIC1_DATA, ICW4_8086);
  io_wait();
  outb(PIC2_DATA, ICW4_8086);
  io_wait();

  // mask all interrupts
  outb(PIC1_DATA, 0xFF);
  outb(PIC2_DATA, 0xFF);
}
