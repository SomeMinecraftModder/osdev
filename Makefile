.POSIX:
.PHONY: celan clean all debug reldebinfo release

include Makefile.config

C_SOURCES = $(wildcard kernel/*.c drivers/*.c cpu/*.c libc/*.c debug/*.c)
HEADERS = $(wildcard kernel/*.h drivers/*.h cpu/*.h libc/*.h debug/*.h)
ASM_SOURCES = $(wildcard boot/*.asm cpu/*.asm drivers/*.asm)
# Nice syntax for file extension replacement
OBJ = ${C_SOURCES:.c=.o} ${ASM_SOURCES:.asm=.o}

all: release

debug: CFLAGS += -g
debug: RELTYPE = Debug
debug: os-image.iso

release: CFLAGS += -Os
release: LDFLAGS += -Os
release: RELTYPE = Release
release: os-image.iso

reldebinfo: CFLAGS += -g -Og
reldebinfo: LDFLAGS += -Og
reldebinfo: RELTYPE = RelWithDebInfo
reldebinfo: os-image.iso

os-image.iso: os-image.elf grub.cfg
	${ECHO} "ISO Build Date: $$(${DATE})" > make.log
	${ECHO} "ISO Build Config: ${RELTYPE}" >> make.log
	${MKDIR} -p iso/boot/grub/
	${CP} os-image.elf iso/boot/
	${CP} grub.cfg iso/boot/grub/
	${ISOMAKER} -o os-image.iso iso

os-image.elf: linker.ld ${OBJ}
	${CC} -T $^ -o os-image.elf ${LDFLAGS}

# Generic rules for wildcards
# To make an object, always compile from its .c
%.o: %.c ${HEADERS}
	${CC} -c $< -o $@ ${CFLAGS}

%.o: %.asm
	${NASM} $< -o $@ -f elf32

# Alias for common mistakes
celan: clean

clean:
	rm -rf *.bin *.dis *.elf *.o *.iso os-image.elf os-image.iso
	rm -rf boot/*.o cpu/*.o debug/*.o drivers/*.o kernel/*.o libc/*.o iso/
