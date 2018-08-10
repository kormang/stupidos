#include "isr.h"
#include "io.h"
#include "screen.h"

static isr_t interrupt_handlers[ISR_COUNT];

void isr_common_handler (registers_t regs) {
  if (regs.int_no >= 40) { // it is from slave IRQ controller
    outb(0xA0, 0x20); // reset signal to slave
  }
  outb(0x20, 0x20); // reset signal to master
  if (regs.int_no < ISR_COUNT && interrupt_handlers[regs.int_no] != 0) {
    interrupt_handlers[regs.int_no](&regs);
  } else {
    screen_print_hex(regs.int_no);
    screen_print(" No handler!\n");
  }
}

void init_isrs() {
  for (uint32_t i = 0; i < ISR_COUNT; ++i) {
    interrupt_handlers[i] = (isr_t)(void*)0;
  }
}

void register_interrupt_handler(uint8_t irq_num, isr_t isr) {
  interrupt_handlers[irq_num] = isr;
}

asm_isr_t asm_isrs[] = {
  isr0,
  isr1,
  isr2,
  isr3,
  isr4,
  isr5,
  isr6,
  isr7,
  isr8,
  isr9,
  isr10,
  isr11,
  isr12,
  isr13,
  isr14,
  isr15,
  isr16,
  isr17,
  isr18,
  isr19,
  isr20,
  isr21,
  isr22,
  isr23,
  isr24,
  isr25,
  isr26,
  isr27,
  isr28,
  isr29,
  isr30,
  isr31,
  isr32,
  isr33,
  isr34,
  isr35,
  isr36,
  isr37,
  isr38,
  isr39,
  isr40,
  isr41,
  isr42,
  isr43,
  isr44,
  isr45,
  isr46,
  isr47
};
