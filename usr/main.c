#include <stdio.h>
#include <sys.h>

int main() {
	int i;
	i = fork();
	if (i) {
		puts("Hi, I'm parent process and my child has PID: ");
		puthex(i);
		putc('\n');
	} else {
		// you can put some loops here and in the parent code to test multitasking
		puts("Hi, I'm child process, and I'll exit now.\n");
		return 8;
	}

	char c = getc();
	while (c != 'q') {
		putc(c);
		c = getc();
	}
	return 3;
}
