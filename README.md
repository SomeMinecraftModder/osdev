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
- RTC (Real Time Clock)
- And more features.

This OS is based on [cfenollosa/os-tutorial](https://github.com/cfenollosa/os-tutorial) and some other OSes with GPL compatible license.

### Directory structure:
```
├───boot # Boot files, includes GDT, Multiboot headers and Boot file.
├───cpu # Files related to the CPU, like ports or interrupts.
├───drivers # As the name states, are drivers like the keyboard or screen driver.
├───iso # Folder created after successful build. Includes bin file (The OS) and grub.cfg (Grub configuration file).
│   └───boot
│       └───grub
├───kernel # Files directly related to the OS/kernel, like the kernel panic or the kernel itself.
├───libc # A small LibC used by various Kernel components.
└───os-image.iso # Bootable file which includes the OS and Grub bootloader.
```
