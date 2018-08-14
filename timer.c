#include "timer.h"
#include "isr.h"
#include "pic.h"
#include "io.h"
#include "task.h"
#include "screen.h"

static void timer_callback(registers_t* regs) {
  schedule_from_timer(regs);
}

void init_timer(uint32_t frequency) {
  register_interrupt_handler(IRQ(0), &timer_callback);

  uint16_t divisor = 1193180 / frequency;
  outb(0x43, 0x36);

  uint8_t div_low = (uint8_t)(divisor & 0xFF);
  uint8_t div_high = (uint8_t)((divisor >> 8) & 0xFF);

  outb(0x40, div_low);
  outb(0x40, div_high);

  uint16_t mask = inb(PIC1_DATA);
  mask = mask & 0xFE; // enable IRQ 0
  outb(PIC1_DATA, mask);
}
