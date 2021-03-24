#if defined(__linux__)
  #error "This code must be compiled with a cross-compiler"
#elif !defined(__i386__)
  #error "This code must be compiled with an x86-elf compiler"
#endif

#include "../drivers/screen.h"
#include "../drivers/serial.h"
#include "../drivers/reboot.h"
#include "../libc/string.h"
#include "../cpu/detect.h"
#include "../libc/mem.h"
#include "../cpu/isr.h"
#include "kernel.h"
#include "panic.h"

void kernel_main() {
    clear_screen();
    isr_install();
    irq_install();
    serial_install(); // COM1
    write_serial("\nCOM1 successfully initialized.\n\n");
    
    kprint("Boot success.\n"
        "\n");

    kprint("Type something, it will go through the kernel\n"
        "Type \"end\" to halt the CPU, \"page\" to request a kmalloc()\n"
        "\"clear\" to clear the screen, \"cpuinfo\" to get information about your CPU or\n"
        "\"reboot\" to reboot your computer (There is no shutdown yet). \n> ");
}

void user_input(char *input) {
    int dis_print = 0;
    if (strcmp(input, "end") == 0) {
        kprint("Stopping the CPU. Bye!\n");
        halt();
    } else if (strcmp(input, "page") == 0) {
        uint32_t phys_addr;
        uint32_t page = kmalloc(1000, 1, &phys_addr);
        char page_str[16] = "";
        hex_to_ascii(page, page_str);
        char phys_str[16] = "";
        hex_to_ascii(phys_addr, phys_str);
        kprint("Page: ");
        kprint(page_str);
        kprint(", physical address: ");
        kprint(phys_str);
        kprint("\n");
    } else if (strcmp(input, "clear") == 0) {
        clear_screen();
        dis_print = 1;
    } else if (strcmp(input, "reboot") == 0) {
        reboot();
    } else if (strcmp(input, "cpuinfo") == 0) {
        cpudetect();
    }
    if (dis_print == 0) {
        kprint("You said: ");
        kprint(input);
        kprint("\n> ");
    } else if (dis_print == 1) {
        kprint("> ");
    } else {
        PANIC("No valid value on dis_print variable.", "kernel.c", 37);
    }
}
