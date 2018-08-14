extern process_stack_start
extern fork
extern on_task_returned
extern process_main
extern screen_put_char
extern screen_print_hex
extern next_task
extern next_directory
extern next_task_dir_phys_addr
extern next_task_esp
extern current_task
extern current_directory

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

global read_eip
read_eip:
	pop eax
	jmp eax

global start_process
start_process:
	push ebx
	mov ebx, esp ; ebx = kernel stack

	mov esp, [process_stack_start] ; moved stack to user pages

	call fork

	cmp eax, 0
	je .start_of_process
	; kernel

	mov esp, ebx ; restore kernel stack

	pop ebx
	ret

.start_of_process:
	; user
	; we want to jump to user start function
	; but return from it to on_task_returned

	push on_task_returned
	jmp process_main

global switch_task_dispatched
switch_task_dispatched:
	cli

	mov eax, [next_task_dir_phys_addr]
	mov cr3, eax ; page dir switched
	mov esp, [next_task_esp] ; stack switched, we're good to go

	mov eax, [next_directory]
	mov [current_directory], eax
	mov eax, [next_task]
	mov [current_task], eax ; now we can say that current_task is next_task

	push dword [eax + 12*4] ; offset of eip in task_t
	push dword [eax + 13*4] ; offset of eflags in task_t

	mov edi, [eax + 2*4]
	mov esi, [eax + 3*4]
	mov ebp, [eax + 4*4]
	mov ebx, [eax + 6*4]
	mov edx, [eax + 7*4]
	mov ecx, [eax + 8*4]
	mov eax, [eax + 9*4]

	popf
	sti
	ret

global switch_to_user_mode
switch_to_user_mode:
	cli
	;mov ax, 0x23 ; user data segment 0x20 | 0x3 - ring 3
	;mov ds, ax
	;mov es, ax
	;mov fs, ax
	;mov gs, ax

	pop ecx ; save return address
	mov eax, esp ; save stack pointer
	push 0x10 ; stack segment selector = data segment
	push eax ; restore stack after iret
	pushf ; eflags
	push 0x8 ; user code segment 0x18 | 0x3 ring 3
	push ecx ; push return address
	iret

