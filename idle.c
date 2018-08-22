#include "idle.h"
#include "screen.h"
#include "pic.h"
#include "io.h"

// static void blabla(char c) {
//   for (int i = 0; i < 100; ++i) {
//     screen_put_char(c);
//     screen_put_char('\b');
//   }
// }

// static void print_waiting_clock() {
  // blabla('/');
  // blabla('|');
  // blabla('\\');
  // blabla('-');
  // blabla('/');
  // blabla('|');
  // blabla('\\');
  // blabla('-');
  // blabla('/');
// }

void kernel_idle_task() {

  // for some reason this IRQ gets re-enabled during initialization
  // this is hacky fix
  uint16_t mask = inb(PIC1_DATA);
  mask = mask | 0x08; // disable IRQ 7
  outb(PIC1_DATA, mask);

  screen_print("Starting kernel idle loop.");
  for(;;){
    // print_waiting_clock();
  }
}
