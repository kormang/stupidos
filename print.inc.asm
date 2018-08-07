[BITS 32]

VIDEO_MEMORY equ 0xb8000
VIDEO_MEMORY_SIZE equ 2000 ; 80 * 25
VIDEO_MEMORY_FOURTH_ROW equ 0xb81e0

section .text

sprint:
	mov edx, dword [pos]
	
sprint_loop:
	mov al, [ebx]
	mov ah, 0xd
	cmp al, 0
	je sprint_done
	mov [edx], ax
	add ebx, 1
	add edx, 2
	jmp sprint_loop

sprint_done:
	mov dword [pos], edx	
	ret

clear_screen:
	mov edi, VIDEO_MEMORY
	mov ecx, VIDEO_MEMORY_SIZE
	mov ax, 0x0d20 ; magenta (0xd) + ascii space (0x20)
	rep stosw
	mov dword [pos], VIDEO_MEMORY
	ret

section .data

pos dd VIDEO_MEMORY_FOURTH_ROW

