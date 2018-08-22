#include "init.h"
#include "gdt.h"
#include "pic.h"
#include "interrupts.h"
#include "keyboard.h"
#include "paging.h"
#include "timer.h"
#include "syscall.h"
#include "task.h"

void init_kernel() {
  init_gdt();
  pic_remap();
  init_isrs();
  init_idt();
  init_keyboard();
  init_timer(36);
  init_paging();
  init_syscalls();
  init_tasking();
}
