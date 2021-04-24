OUTPUT_FORMAT(elf32-i386)
/* Entry point of our kernel */
ENTRY(_start)

SECTIONS
{
	/* We need 1MB of space at least */
	. = 1M;
	start = .;

	/* Text section */
	.text BLOCK(4K) : ALIGN(4K)
	{
		*(.multiboot)
		*(.text)
	}

	/* Read only data section */
	.rodata BLOCK(4K) : ALIGN(4K)
	{
		*(.rodata)
	}

	/* Data section */
	.data BLOCK(4K) : ALIGN(4K)
	{
		*(.data)
	}

	/* BSS section */
	.bss BLOCK(4K) : ALIGN(4K)
	{
		sbss = .;
		*(.bss)
		ebss = .;
	}

	.kend BLOCK(4K) : ALIGN(4K)
	{
		*(.kend)
	}

	/*
	 * Get rid of unnecessary GCC bits.
	 */
	/DISCARD/ :
	{
		*(.comment)
		*(.eh_frame)
		*(.note.gnu.build-id)
	}

	end = ALIGN(4K);
}
