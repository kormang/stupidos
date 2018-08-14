global load_gdtr
load_gdtr:
	mov eax, [esp+4]
	lgdt [eax]

	mov ax, 0x10 ; set data segment registers
	mov ds, ax
	mov es, ax
	mov fs, ax
	mov gs, ax
	mov ss, ax
	jmp 0x08:.gdtr_loaded ; far jump sets the code segment cs register to 0x8
.gdtr_loaded:
	ret

global load_tr
; load task state register
load_tr:
	mov ax, 0x2B ; 5th selector * 8 bytes | 0x3 (Request Priviledge Level from ring 3 to 0)
	ltr ax
	ret

