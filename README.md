# PawsOS
A simple OS, which has various features:
-   IRQs
-   ISR
-   GDT
-   Ports
-   Serial ports (COM1, COM2, COM3, COM4)
-   Keyboard
-   Multiboot (Can be booted by GNU Grub)
-   RTC (Real Time Clock)
-   ACPI (Can shut down your computer)
-   Paging
-   Memory manager
-   And more.

This OS is based on [cfenollosa/os-tutorial](https://github.com/cfenollosa/os-tutorial) and some other OSes with GPL compatible license.


![HSrny1Ot](https://user-images.githubusercontent.com/62727185/121619214-da50ce80-ca2d-11eb-84ac-b67f67eeddb3.gif)

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
Firstly, you need to build an `i386-elf-gcc` cross-compiler, you can get build instructions [here](https://wiki.osdev.org/GCC_Cross-Compiler) (WARNING: The build process of your cross-compiler can take various hours depending on your Hardware).

There are various GCC versions available, here is a compatibility list:
-   \[✔\] GCC 11.1.0
-   \[✔\] GCC 10.2.0
-   \[✔\] GCC 8.4.0
-   \[❌\] GCC 4.9.1

Then, you will need the Grub bootloader and the image builder which are generally distributed on 2 different packages:
-   `grub-pc-bin`
-   `grub-mkrescue`

You can get that packages from your package manager.

And get CMake from your package manager or [download](https://cmake.org/download/) it manually.

Finally, you will need the NASM compiler, you can get it from your distro package manager.

Add all these to your operating system path.

Then build following this steps:

1. Create a directory called `build` in the source directory.
2. Change to that directory.
3. Run `cmake -DCMAKE_BUILD_TYPE=Release ..`.
4. Run `make` and wait until build finishes.

You will get two files after a successful build:
-   os-image.elf
-   os-image.iso

You can use the `os-image.elf` file for debugging purposes.

Not only that, but you can use the `os-image.iso` file to boot using a bootloader (GNU Grub by default, but you can use another Multiboot compliant bootloader).

The default output format is `elf32-i386`.
