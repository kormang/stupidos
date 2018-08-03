; kernel.asm
[BITS 32]
[ORG 0x100000]

kernel_code:
	jmp start

%include "print.inc.asm"

start:
	cli ; it should already be disabled by bootloader, but just in case
	mov esp, 0x10000; at least 15kB of stack after boot loader and initial kernel position

	mov ebx, hello_from_kernel_asm
	call sprint

	hlt

hello_from_kernel_asm db 'Hello from kernel.asm !!!', 0

