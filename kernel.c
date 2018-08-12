/* kernel.c */

#include "screen.h"
#include "init.h"
#include "keyboard.h"

void kernel_main () {
	const char *msg = "Hello from kernel.c\n";

	screen_clear();
	screen_print(msg);
	init_kernel();
	set_on_keypressed(&screen_put_char);

	for (;;) {}
}
