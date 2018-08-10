#ifndef INTERRUPTS_H
#define INTERRUPTS_H
#include "io.h"
#include "isr.h"

#define IDT_GATE_TYPE_TASK_32 0x5
#define IDT_GATE_TYPE_INTERRUPT_16 0x6
#define IDT_GATE_TYPE_TRAP_16 0x7
#define IDT_GATE_TYPE_INTERRUPT_32 0xE
#define IDT_GATE_TYPE_TRAP_32 0xF

struct idt_entry {
  uint16_t offset_lower;
  uint16_t selector;
  uint8_t zero;
  union {
    uint8_t flags;
    struct {
      uint8_t type :4;
      uint8_t storage_segment :1; // 0 for interrupt and trap gates
      uint8_t descriptor_privilege_level :2;
      uint8_t present :1; // 0 for unused
    };
  };
  uint16_t offset_higher;
}__attribute__((packed));

struct idt_ptr {
  uint16_t limit;
  uint32_t base;
}__attribute__((packed));

void init_idt();

#endif
