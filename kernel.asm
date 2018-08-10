; kernel.asm
[BITS 32]

global start
extern kernel_main

start:
	cli ; it should already be disabled by bootloader, but just in case
	mov esp, stack_start ;0x10000; at least 15kB of stack after boot loader and initial kernel position

;	mov ebx, hello_from_kernel_asm
;	call sprint

	call kernel_main

	hlt

;%include "print.inc.asm"

section .data

;hello_from_kernel_asm db 'Hello from kernel.asm !!!', 0

section .bss
resb 0x2000
stack_start:
