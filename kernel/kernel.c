#if defined(__linux__)
  #error "This code must be compiled with a cross-compiler"
#elif !defined(__i386__)
  #error "This code must be compiled with an x86-elf compiler"
#endif

#include "../drivers/screen.h"
#include "../drivers/serial.h"
#include "../drivers/acpi.h"
#include "../cpu/isr.h"
#include "kernel.h"
#include "panic.h"
#include "shell.h"

void kernel_main() {
    // Clear the screen
    clear_screen();
    // Enable ISRs (Interrupt Service Routines)
    isr_install();
    // Install interrupts
    irq_install();
    // Enable ACPI (Advanced Configuration and Power Interface)
    acpi_install();
    // Enable serial port (COM1)
    serial_install();
    write_serial("\nCOM1 successfully initialized.\n\r\n");

    kprint("Boot success.\n"
        "\n");

    kprint("Type something, it will go through the kernel\n"
        "Type \"help\" to get started\n> ");
}

void user_input(char *input) {
    shell(input);
}
