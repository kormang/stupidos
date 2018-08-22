#include <stdio.h>
#include <sys.h>

int main() {
	int i;
	i = fork();
	if (i) {
		puts("Hi, my child is ");
		puthex(i);
		putc('\n');
	} else {
		puts("I'm child, and I'll exit now.\n");
		return 8;
	}

	char c = getc();
	while (c != 'q') {
		putc(c);
		c = getc();
	}
	return 3;
}

