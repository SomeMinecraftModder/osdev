include Makefile.config

.POSIX:
.PHONY: celan clean

C_SOURCES = $(wildcard kernel/*.c drivers/*.c cpu/*.c libc/*.c)
HEADERS = $(wildcard kernel/*.h drivers/*.h cpu/*.h libc/*.h)
# Nice syntax for file extension replacement
OBJ = ${C_SOURCES:.c=.o cpu/interrupt.o boot/boot.o boot/header.o boot/gdt.o}

os-image.iso: os-image.bin grub.cfg
	${ECHO} "ISO Build Date: $$(${DATE})" > make.log
	${MKDIR} -p iso/boot/grub/
	${CP} os-image.bin iso/boot/
	${CP} grub.cfg iso/boot/grub/
	${ISOMAKER} -o os-image.iso iso

os-image.bin: linker.ld ${OBJ}
	${LD} -T $^ -o os-image.bin -nostdlib

# Generic rules for wildcards
# To make an object, always compile from its .c
%.o: %.c ${HEADERS}
	${CC} ${CFLAGS} -Os -c $< -o $@

%.o: %.asm
	${NASM} $< -f elf -o $@

%.bin: %.asm
	${NASM} $< -f bin -o $@

# Alias for common mistakes
celan: clean

clean:
	rm -rf *.bin *.dis *.elf *.o *.iso os-image.bin os-image.iso
	rm -rf boot/*.bin boot/*.o cpu/*.o drivers/*.o kernel/*.o libc/*.o isodir/
