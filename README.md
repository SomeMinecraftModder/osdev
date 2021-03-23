# PawsOS
A simple OS which supports various features:
- IRQs
- ISR
- GDT
- Ports
- Serial ports (COM1, COM2, COM3, COM4)
- Keyboard
- Multiboot (Can be booted by GNU Grub)
- Small memory manager (unfinished)
- And more features.

This OS is based on [cfenollosa/os-tutorial](https://github.com/cfenollosa/os-tutorial) and some others OSes with GPL compatible license.

Directory structure:
```
├───boot # Boot files, includes GDT, Multiboot headers and Boot file.
├───cpu # Files related to the CPU, like ports or interrupts.
├───drivers # As the name says, are drivers like the keyboard or screen driver.
├───iso # Folder created after successful build. Includes bin file (The kernel) and grub.cfg (Grub configuration file).
│   └───boot
│       └───grub
├───kernel # Files directly related to the Kernel, like the kernel panic or the kernel itself.
└───libc # A small LibC used by various Kernel components.
```
