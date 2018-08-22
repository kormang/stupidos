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
extern kernel_idle_task

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

	; after kernel forks to first process all that is left is to respond to ISRs
	; and even if there is something, it can be called from kernel_idle_task
	call kernel_idle_task

	; kernel_idle_task should never return
	; we can't really get back to original kernel stack and code
	; I leave this here only for reference
	; original kernel stack is probably overwritten by ISRs
	mov esp, ebx ; restore kernel stack

	pop ebx
	ret

.start_of_process:
	; user
	; we want to jump to user start function
	; but return from it to on_task_returned

	push on_task_returned
	jmp process_main

global switch_task_dispatched_to_user
switch_task_dispatched_to_user:
	; (page_dir_phys_addr, registers)
	add esp, 4 ; take off return address

	; we can switch dir because we are on kernel stack, and it is mapped to all dirs
	pop eax ; take off page dir phys addr
	mov cr3, eax ; page dir switched

	pop eax
	mov ds, ax
	mov es, ax
	mov fs, ax
	mov gs, ax

	pop edi
	pop esi
	pop ebp
	add esp, 4 ;pop esp
	pop ebx
	pop edx
	pop ecx
	pop eax
	add esp, 8 ; intno + errcode

	iret

global switch_task_dispatched_kernel
switch_task_dispatched_kernel:
	; (page_dir_phys_addr, registers_ptr)
	mov eax, [esp + 4] ; take off page dir phys addr
	mov cr3, eax ; page dir switched

	mov eax, [esp + 8] ; eax points to registers, which should be in kernel memory, mapped to all dirs

	; ds should be the same, we're not switching priviledge level
	; mov ecx, [eax] ; take first field of registers - ds
	; mov ds, cx
	; mov es, cx
	; mov fs, cx
	; mov gs, cx

	mov edi, [eax + 4]
	mov esi, [eax + 8]
	mov ebp, [eax + 12]
	mov esp, [eax + 56] ; prev_esp
	mov ebx, [eax + 20]
	mov edx, [eax + 24]
	mov ecx, [eax + 28]

	push dword [eax + 44] ; push return address to new stack
	push dword [eax + 52] ; push eflags

	mov eax, [eax + 32] ; finally restore eax

	popf ; pop eflags from new stack
	ret ; pop return address and continue with task

global switch_to_user_mode
switch_to_user_mode:
	cli
	mov ax, 0x23 ; user data segment 0x20 | 0x3 - ring 3
	mov ds, ax
	mov es, ax
	mov fs, ax
	mov gs, ax

	mov ecx, [esp + 4] ; save return address that is passed as parameter
	mov eax, esp ; save stack pointer
	push dword 0x23 ; stack segment selector = data segment
	push eax ; restore stack after iret
	pushf ; eflags
	push dword 0x1B ; user code segment 0x18 | 0x3 ring 3
	push ecx ; push return address
	iret

