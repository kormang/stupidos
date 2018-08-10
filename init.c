#include "init.h"
#include "pic.h"
#include "interrupts.h"
#include "keyboard.h"

void init_kernel() {
  pic_remap();
  init_isrs();
  init_idt();
  init_keyboard();

  __asm__ volatile ("sti");
}
