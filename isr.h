#ifndef ISR_H
#define ISR_H

//
// isr.h -- Interface and structures for high level interrupt service routines.
// Part of this code is modified from Bran's kernel development tutorials.
// Rewritten for JamesM's kernel development tutorials.
//

#include "common.h"

#define ISR_COUNT 48
#define IRQ_COUNT 16
#define SYSCALL_INT_NO 128

// represent registers pushed by jump to ISR code (assembly0)
typedef struct registers
{
   // offset: 0
   uint32_t ds;                  // Data segment selector
   // offset: 4,  8,  12,  16,      20,  24,  28,  32
   uint32_t edi, esi, ebp, isr_esp, ebx, edx, ecx, eax; // Pushed by pusha.
   // offset: 36,   40
   uint32_t int_no, err_code;    // Interrupt number and error code (if applicable)
   // offset: 44, 48, 52,    56,       60
   uint32_t eip, cs, eflags, prev_esp, ss; // Pushed by the processor automatically.
} registers_t;

typedef void (*isr_t)(registers_t*);

void init_isrs();
void register_interrupt_handler(uint8_t irq_num, isr_t isr);

void print_regs(registers_t* regs);

extern void isr0();
extern void isr1();
extern void isr2();
extern void isr3();
extern void isr4();
extern void isr5();
extern void isr6();
extern void isr7();
extern void isr8();
extern void isr9();
extern void isr10();
extern void isr11();
extern void isr12();
extern void isr13();
extern void isr14();
extern void isr15();
extern void isr16();
extern void isr17();
extern void isr18();
extern void isr19();
extern void isr20();
extern void isr21();
extern void isr22();
extern void isr23();
extern void isr24();
extern void isr25();
extern void isr26();
extern void isr27();
extern void isr28();
extern void isr29();
extern void isr30();
extern void isr31();
extern void isr32();
extern void isr33();
extern void isr34();
extern void isr35();
extern void isr36();
extern void isr37();
extern void isr38();
extern void isr39();
extern void isr40();
extern void isr41();
extern void isr42();
extern void isr43();
extern void isr44();
extern void isr45();
extern void isr46();
extern void isr47();

extern void isr128();

typedef void (*asm_isr_t)();
extern asm_isr_t asm_isrs[];

#endif
