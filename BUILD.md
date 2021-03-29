# BUILDING
Firstly, you need to build an `i386-elf-gcc` cross-compiler, you can get build instructions [here](https://wiki.osdev.org/GCC_Cross-Compiler) (WARNING: The build proccess of your cross-compiler can take various hours depending on your Hardware).

After cross-compiler successful build, you build the OS with the following instructions:

1. Modify the `Makefile.config` file to point to your cross-compiler.
2. Build using an POSIX compliant `make`.

You will get two files after an successful build:
- os-image.elf
- os-image.iso

You can use the `os-image.elf` file to debug and/or to boot OS directly (QEMU: -kernel option).

You can use the `os-image.iso` to boot using an bootloader (GNU Grub by default, but you can use another Multiboot compliant bootloader).

The default output format is `elf32-i386` but you can change it in `linker.ld` (not recommended).
