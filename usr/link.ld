/* link.ld */
OUTPUT_FORMAT(binary)
ENTRY(start)
SECTIONS
{
	. = 0x10000000; /* set location counter to user-mode start address */
	.text : { *(.text) }
	.data : { *(.data) }
	.bss : { *(.bss) } /* BSS last so it can expand */
}
