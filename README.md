# PawsOS
A simple OS which have various features:
-   IRQs
-   ISR
-   GDT
-   Ports
-   Serial ports (COM1, COM2, COM3, COM4)
-   Keyboard
-   Multiboot (Can be booted by GNU Grub)
-   RTC (Real Time Clock)
-   ACPI (Can shutdown your computer)
-   Paging
-   Memory manager
-   And more.

This OS is based on [cfenollosa/os-tutorial](https://github.com/cfenollosa/os-tutorial) and some other OSes with GPL compatible license.


![jg6W8GIj](https://user-images.githubusercontent.com/62727185/116011187-595a8680-a5e9-11eb-8ae5-bf69024c8e22.gif)

## Directory structure
```lang-none
┌───boot # Boot files, includes GDT, Multiboot headers and Boot file.
├───cpu # Files related to the CPU, like ports or interrupts.
├───debug # Files used for debugging purposes.
├───drivers # As the name states, are drivers like the keyboard or screen driver.
├───iso # Folder created after successful build. Includes bin file (The OS) and grub.cfg (Grub configuration file).
│   └───boot
│       └───grub
├───kernel # Files directly related to the OS/kernel, like the kernel panic or the kernel itself.
├───libc # A small LibC used by various components on the OS.
└───os-image.iso # Bootable file created after successful build. Includes the OS and the bootloader (Grub).
```

## Building
Firstly, you need to build an `i386-elf-gcc` cross-compiler, you can get build instructions [here](https://wiki.osdev.org/GCC_Cross-Compiler) (WARNING: The build proccess of your cross-compiler can take various hours depending on your Hardware).

There are various GCC versions available, here is a compatibility list:
-   \[✔\] GCC 11.1.0
-   \[✔\] GCC 10.2.0
-   \[✔\] GCC 8.4.0
-   \[✔\] GCC 4.9.1

Then, you will need Grub bootloader and the image builder which are generally distributed on 2 different packages:
-   `grub-pc-bin`
-   `grub-mkrescue`

You can get that packages from your distro package manager.

Finally, you will need the NASM compiler, you can get it from your distro package manager.

After cross-compiler successful build, you build the OS with the following instructions:

1.  Modify the `Makefile.config` file to point to your cross-compiler. [This](https://github.com/Sebastian-byte/osdev/blob/main/Makefile.config#L11) line.
2.  Modify `Makefile.config` to point your `nasm` executable. [This](https://github.com/Sebastian-byte/osdev/blob/main/Makefile.config#L14) line.
3.  Modify `Makefile.config` again to point your `grub-mkrescue` executable. [This](https://github.com/Sebastian-byte/osdev/blob/main/Makefile.config#L17) line.
4.  Build using an POSIX compliant `make`.

You will get two files after an successful build:
-   os-image.elf
-   os-image.iso

You can use the `os-image.elf` file for debugging purposes.

You can use the `os-image.iso` file to boot using a bootloader (GNU Grub by default, but you can use another Multiboot compliant bootloader).

The default output format is `elf32-i386`.
