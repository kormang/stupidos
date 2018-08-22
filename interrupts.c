#include "interrupts.h"
#include "screen.h"

#define IDT_SIZE 256
static struct idt_entry IDT[IDT_SIZE];
static struct idt_ptr IDT_ptr;

static inline void load_idt(struct idt_ptr* idt_ptr_ptr) {
  __asm__ volatile ("lidt (%%eax)": : "a"(idt_ptr_ptr));
}

static void init_idt_entry(uint32_t index) {
  struct idt_entry* idte = &IDT[index];
  asm_isr_t isr = asm_isrs[index];
  uint32_t offset = (uint32_t)isr;
  idte->offset_lower = (offset & 0xFFFF);
  idte->selector = 0x08; // code segment from GDT
  idte->zero = 0;
  idte->flags = 0x8E;
  idte->offset_higher = (offset >> 16) & 0xFFFF;
}

void init_idt() {

  IDT_ptr.limit = sizeof(IDT) - 1;
  IDT_ptr.base = (uint32_t)&IDT;

  for (uint32_t i = 0; i < IDT_SIZE; i++) {
    init_idt_entry(i);
  }

  init_idt_entry(SYSCALL_INT_NO);
  IDT[SYSCALL_INT_NO].descriptor_privilege_level = 3; // accessible from user-mode

  load_idt(&IDT_ptr);
}
