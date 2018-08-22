#include "syscall.h"
#include "screen.h"
#include "keyboard.h"
#include "task.h"
#include "string.h"

typedef void (*syscall_handler_t)(registers_t* regs);

void sc_putc(registers_t* regs) {
  uint32_t c = regs->ecx;
  screen_put_char(c);
}

void sc_puts(registers_t* regs) {
  uint32_t c = regs->ecx;
  screen_print((char*)c);
}

void sc_puthex(registers_t* regs) {
  uint32_t c = regs->ecx;
  screen_print_hex(c);
}

void on_key_pressed(char c) {
  task_t* t = find_task_waiting(WAITING_KEYBOARD);
  if (t) {
    t->registers.eax = c;
    t->waiting_input &= ~WAITING_KEYBOARD;
  }
}

void sc_getc(registers_t* regs) {
  current_task->waiting_input |= WAITING_KEYBOARD;
  switch_task_from_isr(regs);
}

void sc_exit(registers_t* regs) {
  extern uint32_t kernel_dispatcher_stack_start;
  uint32_t return_value = regs->ecx;
  regs->eip = (uint32_t)&on_task_returned;
  regs->cs = 0x8;
  regs->ss = 0x10;
  regs->ds = 0x10;
  regs->prev_esp = (uint32_t)&kernel_dispatcher_stack_start;
  regs->eax = return_value;
}

void sc_fork(registers_t* regs) {
  fork_user(regs);
}

#define SC_HANDLERS_COUNT 16
syscall_handler_t sc_handlers[SC_HANDLERS_COUNT];

void syscall_handler (registers_t* regs) {
  uint32_t syscall_no = regs->eax;
  // print_regs(regs);

  if (syscall_no >= 0 && syscall_no < SC_HANDLERS_COUNT) {
    syscall_handler_t sch = sc_handlers[syscall_no];
    if (sch) {
      sch(regs);
    }
  }
}

void init_syscalls() {
  memset(sc_handlers, 0, sizeof(sc_handlers));

  sc_handlers[0] = sc_getc;
  sc_handlers[1] = sc_putc;
  sc_handlers[3] = sc_puts;
  sc_handlers[4] = sc_puthex;
  sc_handlers[5] = sc_fork;
  sc_handlers[15] = sc_exit;

  set_on_keypressed(&on_key_pressed);

  register_interrupt_handler(SYSCALL_INT_NO, &syscall_handler);
}
