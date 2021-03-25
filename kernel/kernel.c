#if defined(__linux__)
  #error "This code must be compiled with a cross-compiler"
#elif !defined(__i386__)
  #error "This code must be compiled with an x86-elf compiler"
#endif

#include "../drivers/screen.h"
#include "../drivers/serial.h"
#include "../cpu/isr.h"
#include "kernel.h"
#include "panic.h"
#include "shell.h"

void kernel_main() {
    clear_screen();
    isr_install();
    irq_install();
    serial_install(); // COM1
    write_serial("\nCOM1 successfully initialized.\n\n");

    kprint("Boot success.\n"
        "\n");

    kprint("Type something, it will go through the kernel\n"
        "Type \"help\" to get started\n> ");
}

void user_input(char *input) {
    shell(input);
}
