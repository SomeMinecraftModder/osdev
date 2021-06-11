#if defined(__linux__)
    #error "This code must be compiled with a cross-compiler"
#elif !defined(__i386__)
    #error "This code must be compiled with an x86-elf compiler"
#endif

#include "../cpu/isr.h"
#include "../cpu/paging.h"
#include "../cpu/ports.h"
#include "../drivers/acpi.h"
#include "../drivers/screen.h"
#include "../drivers/serial.h"
#include <signal.h>

void kernel_main(uint32_t addr) {
    // Point to Multiboot info table and clear screen
    init_video(addr);
    // Set IDT gates, remap the PIC and install the IRQs
    isr_install();
    /* Enable interrupts, set the timer interrupt (IRQ0)
     * and the keyboard interrupt (IRQ1) */
    irq_install();
    // Map ACPI tables and send the enable command
    acpi_install();
    // Enable paging and identity map kernel and video memory
    init_paging();
    // Map every possible signal
    init_signal();
    // Test serial port (COM1)
    serial_install();

    kprint("Boot success.\n"
           "\n");

    kprint("Copyright (C) 2020-2021 The PawsOS Team\n"
           "This program comes with ABSOLUTELY NO WARRANTY.\n"
           "This is free software, type 'ver' for details.\n");

    kprint("\nType 'help' to get started\n> ");
}
