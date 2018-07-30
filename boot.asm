; boot.asm
[ORG 0x7c00]
	xor ax, ax
	mov ds, ax

	mov si, msg
	call bios_print

hang:
	jmp hang

bios_print:
	lodsb
	or al, al ; if zero then end of string reached
	jz done
	mov ah, 0x0E
	int 0x10
	jmp bios_print
done:
	ret

msg db 'Hello World', 13, 10, 0
	times 510-($-$$) db 0
	db 0x55
	db 0xAA

