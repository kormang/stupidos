#include "init.h"
#include "pic.h"
#include "interrupts.h"
#include "keyboard.h"
#include "paging.h"

void init_kernel() {
  pic_remap();
  init_isrs();
  init_idt();
  init_keyboard();
  init_paging();

  __asm__ volatile ("sti");
}
