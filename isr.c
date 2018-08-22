#include "isr.h"
#include "io.h"
#include "pic.h"
#include "screen.h"

static isr_t interrupt_handlers[ISR_COUNT];

void isr_common_handler (registers_t regs) {
  if (regs.int_no >= PIC1_OFFSET_VECTOR) {
    if (regs.int_no >= PIC2_OFFSET_VECTOR) { // it is from slave IRQ controller
      outb(0xA0, 0x20); // reset signal to slave
    }
    outb(0x20, 0x20); // reset signal to master
  }

  if ((regs.int_no < ISR_COUNT || regs.int_no == SYSCALL_INT_NO) && interrupt_handlers[regs.int_no] != 0) {
    interrupt_handlers[regs.int_no](&regs);
  } else {
    screen_print_hex(regs.int_no);
    screen_print(" No handler!\n");
    __asm__("cli");
    __asm__("hlt");
  }
}

void init_isrs() {
  for (uint32_t i = 0; i < ISR_COUNT; ++i) {
    interrupt_handlers[i] = (isr_t)(void*)0;
  }

  asm_isrs[SYSCALL_INT_NO] = isr128;
}

void register_interrupt_handler(uint8_t irq_num, isr_t isr) {
  interrupt_handlers[irq_num] = isr;
}

void print_regs(registers_t* regs) {
  screen_print("ds: "); screen_print_hex(regs->ds); screen_put_char('\n');
  screen_print("edi: "); screen_print_hex(regs->edi); screen_put_char('\n');
  screen_print("esi: "); screen_print_hex(regs->esi); screen_put_char('\n');
  screen_print("ebp: "); screen_print_hex(regs->ebp); screen_put_char('\n');
  screen_print("isr_esp: "); screen_print_hex(regs->isr_esp); screen_put_char('\n');
  screen_print("ebx: "); screen_print_hex(regs->ebx); screen_put_char('\n');
  screen_print("edx: "); screen_print_hex(regs->edx); screen_put_char('\n');
  screen_print("ecx: "); screen_print_hex(regs->ecx); screen_put_char('\n');
  screen_print("eax: "); screen_print_hex(regs->eax); screen_put_char('\n');
  screen_print("int_no: "); screen_print_hex(regs->int_no); screen_put_char('\n');
  screen_print("err_code: "); screen_print_hex(regs->err_code); screen_put_char('\n');
  screen_print("eip: "); screen_print_hex(regs->eip); screen_put_char('\n');
  screen_print("cs: "); screen_print_hex(regs->cs); screen_put_char('\n');
  screen_print("eflags: "); screen_print_hex(regs->eflags); screen_put_char('\n');
  screen_print("prev_esp: "); screen_print_hex(regs->prev_esp); screen_put_char('\n');
  screen_print("ss: "); screen_print_hex(regs->ss); screen_put_char('\n');
}

asm_isr_t asm_isrs[ISR_COUNT] = {
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
