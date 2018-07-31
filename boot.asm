; boot.asm
[BITS 16]
[ORG 0x7c00]

; 00000 - 003ff   IVT
; 00400 - 004ff   BDA
; 00500 - 0052f   The Petch Zone :)
; 00530 - 07bff   Stack
; 07c00 - 07dff   IPL - initial program loader
; 07e00 - 08dff   Kernel
; 08e00 - fffff   Free

start:
	xor ax, ax
	mov ds, ax ; data segment starts from 0
	mov es, ax

	mov bx, 0x7c00
	mov ss, ax
	mov sp, bx

	cld ; set positive direction	

	mov si, msg_loading_kernel
	call bios_print

	call load_kernel

	mov si, msg_starting_kernel
	call bios_print

	;jmp 0:0x7e00 ; long jump to kernel

hang:
	jmp hang

bios_print:
	lodsb
	or al, al ; if zero then end of string reached
	jz done_print
	mov ah, 0x0E
	int 0x10
	jmp bios_print
done_print:
	ret

load_kernel:
	xor ax, ax
	mov es, ax
	mov bx, 0x7e00 ; read destination ES:BX = 0:7e00h
	mov ah, 2 ; read operation
	mov al, 4 ; read 4 sectors
	mov ch, 0 ; cylinder 0
	mov dh, 0 ; head 0
	mov cl, 2 ; sector 2

	int 13h ; ask BIOS to read the sector

	jnc done_load_kernel ; carry bit is set on error

	mov si, msg_failed
	call bios_print
	jmp hang	
	
done_load_kernel:
	ret


msg_loading_kernel db 'Loading kernel...', 13, 10, 0
msg_failed db 'FAILED!', 0
msg_starting_kernel db 'Starting kernel...', 13, 10, 0
times 510-($-$$) db 0
db 0x55
db 0xAA

