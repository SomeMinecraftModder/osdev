OUTPUT_FORMAT(elf32-i386)
ENTRY(_start)

SECTIONS {
	. = 1M;
	start = .;

	.text BLOCK(4K) : ALIGN(4K) {
		*(.multiboot)
		*(.text)
	}

	.rodata BLOCK(4K) : ALIGN(4K) {
		*(.rodata)
	}

	.data BLOCK(4K) : ALIGN(4K) {
		*(.data)
	}

	.bss BLOCK(4K) : ALIGN(4K) {
		sbss = .;
		*(.bss)
		ebss = .;
	}

	/* Get rid of unnecessary GCC bits */
	/DISCARD/ : {
		*(.comment)
		*(.eh_frame)
		*(.note.gnu.build-id)
	}

	end = ALIGN(4K);
}
