
global copy_frame

copy_frame:
	push ebx
	pushf ; save eflags
	cli ; don't get interrupted

	mov ebx, [esp+12] ; src
	mov ecx, [esp+16] ; dst

	; disable paging
	mov edx, cr0
	and edx, 0x7fffffff
	mov cr0, edx

	mov edx, 1024

.loop:
	mov eax, [ebx]
	mov [ecx], eax
	add ebx, 4
	add ecx, 4
	dec edx
	jnz .loop

	; enable paging
	mov edx, cr0
	or edx, 0x80000000
	mov cr0, edx

	popf
	pop ebx
	ret

