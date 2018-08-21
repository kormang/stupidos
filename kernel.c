/* kernel.c */

#include "screen.h"
#include "init.h"
#include "keyboard.h"
#include "isr.h"
#include "task.h"

void gp_fault_handler(registers_t* regs) {
	screen_print("GPF: "); screen_print_hex(regs->err_code); screen_put_char('\n');
	screen_print_hex(regs->cs); screen_put_char(' '); screen_print_hex(regs->ss);
	screen_put_char(' '); screen_print_hex(regs->ds); screen_put_char('\n');
	screen_print_hex(regs->eip); screen_put_char(' ') ;screen_print_hex((uint32_t)&screen_put_char); screen_put_char('\n');

	__asm__ volatile ("cli");
	__asm__ volatile ("hlt");
}

void kernel_main () {
	const char *msg = "Hello from kernel.c\n";

	screen_clear();
	screen_print(msg);
	init_kernel();
	// set_on_keypressed(&screen_put_char);

	register_interrupt_handler(0xD, &gp_fault_handler);

	__asm__ volatile ("sti");
	screen_print("start process");
	start_process();
	// we should never reach this line, and should instead be in kernel_idle_task


	for (;;) {}
}
