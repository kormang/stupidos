/* kernel.c */

#include "./screen.h"

void kernel_main () {
	const char *msg = "Hello from kernel.c\r\n"
		"Screen can, now, print multiline text and .\b backspace,\n"
		"and\ttabs\tare\nalso\tprintable.";

	screen_clear();

	screen_print(msg);
}
