#if defined(__linux__)
  #error "This code must be compiled with a cross-compiler"
#elif !defined(__i386__)
  #error "This code must be compiled with an x86-elf compiler"
#endif

#include "../drivers/screen.h"
#include "../drivers/serial.h"
#include "../drivers/acpi.h"
#include "../libc/mem.h"
#include "../cpu/isr.h"
#include "kernel.h"
#include "shell.h"

void kernel_main(uint32_t addr) {
    extern char sbss, ebss;
    memset(&sbss, 0, &ebss - &sbss);
    // Point to Multiboot info table and clear screen
    video_init(addr);
    // Set IDT gates, remap the PIC and install the IRQs
    isr_install();
    /* Enable interrupts, set the timer interrupt (IRQ0)
     * and the keyboard interrupt (IRQ1) */
    irq_install();
    // Map ACPI tables and send the enable command
    acpi_install();
    // Test serial port (COM1)
    serial_install();

    kprint("Boot success.\n"
        "\n");

    kprint("Type something, it will go through the kernel\n"
        "Type \"help\" to get started\n> ");
}

void user_input(char *input) {
    shell(input);
}
