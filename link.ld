/* link.ld */
OUTPUT_FORMAT(binary)
ENTRY(start)
SECTIONS
{
	. = 0x100000; /* set location counter to kernel start address */
	.text : { *(.text) }
	.data : { *(.data) }
	.bss : { *(.bss) } /* BSS last so it can expand */
}
