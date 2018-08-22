global start
extern main
start:
	call main
	push eax
	call exit

global exit
exit:
	mov eax, 0xF
	mov ecx, [esp + 4]
	int 0x80

global getc
getc:
	mov eax, 0
	int 0x80
	ret

global putc
putc:
	mov eax, 1
	mov ecx, [esp + 4]
	int 0x80
	ret

global fork
fork:
	mov eax, 5
	int 0x80
	ret
