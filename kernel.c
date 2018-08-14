/* kernel.c */

#include "screen.h"
#include "init.h"
#include "keyboard.h"
#include "isr.h"

void kernel_main () {
	const char *msg = "Hello from kernel.c\n";

	screen_clear();
	screen_print(msg);
	init_kernel();
	// set_on_keypressed(&screen_put_char);

	screen_print("Kernel initialized.\n");

	for (int i = 0; i < 6; i++) {
		screen_print("Hello from kernel!!!");
		screen_print("This is multitasking demo!!! This is one process!!!\n");
	}

	for (;;) {}
}
