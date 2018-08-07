/* kernel.c */

#define COLOR_CODE 0x0a // light green
static char* const video_memory = (char*)0xb8000;


void clear_screen () {
	unsigned int i = 0;

	while (i < 80 * 25 * 2) {
		video_memory[i] = ' ';
		video_memory[i+1] = COLOR_CODE;
		i += 2;
	}
}

void prints (const char* const str) {
	unsigned int i = 0;
	unsigned int j = 0;

	while(str[i]) {
		video_memory[j] = str[i];
		video_memory[j+1] = COLOR_CODE;
		++i;
		j += 2;
	}
}

void kernel_main () {
	const char *msg = "Hello from kernel.c";

	clear_screen();

	prints(msg);	
}


