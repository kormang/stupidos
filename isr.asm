
%macro ISR 1
global isr%1
isr%1:
	cli
	push dword 0 ; dummy error code
	push dword %1
	jmp isr_common_stub
%endmacro

; ISR with pushed error code
%macro ISREC 1
global isr%1
isr%1:
	cli
	push dword %1
	jmp isr_common_stub
%endmacro



ISR 0
ISR 1
ISR 2
ISR 3
ISR 4
ISR 5
ISR 6
ISR 7
ISREC 8
ISR 9
ISREC 10
ISREC 11
ISREC 12
ISREC 13
ISREC 14
ISR 15
ISR 16
ISR 17
ISR 18
ISR 19
ISR 20
ISR 21
ISR 22
ISR 23
ISR 24
ISR 25
ISR 26
ISR 27
ISR 28
ISR 29
ISR 30
ISR 31
ISR 32
ISR 33
ISR 34
ISR 35
ISR 36
ISR 37
ISR 38
ISR 39
ISR 40
ISR 41
ISR 42
ISR 43
ISR 44
ISR 45
ISR 46
ISR 47

extern isr_common_handler
isr_common_stub:
	pusha ; all registers: edi,esi,ebp,esp,ebx,edx,ecx,eax
	mov ax, ds
	push eax

	mov ax, 0x10 ; data segment of GDT
	mov ds, ax
	mov es, ax
	mov fs, ax
	mov gs, ax

	call isr_common_handler

	pop ebx
	mov ds, bx
	mov es, bx
	mov fs, bx
	mov gs, bx

	popa
	add esp, 8 ; error code and ISR number pushed by ISR #
	sti
	iret

