#include "../drivers/screen.h"
#include "../drivers/reboot.h"
#include "../debug/printf.h"
#include "../libc/string.h"
#include "../drivers/rtc.h"
#include "../cpu/detect.h"
#include "../libc/mem.h"
#include "panic.h"
#include "shell.h"

void shell(char *input) {
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
        kprintf("Page: %s, physical address: %s\n", page_str, phys_str);
    } else if (strcmp(input, "clear") == 0) {
        clear_screen();
        dis_print = 1;
    } else if (strcmp(input, "reboot") == 0) {
        reboot();
    } else if (strcmp(input, "shutdownvm") == 0) {
        shutdown_vm();
    } else if (strcmp(input, "cpuinfo") == 0) {
        cpudetect();
    } else if (strcmp(input, "rtc") == 0) {
        rtctime();
    } else if (strncmp(input, "echo ", 4) == 0) {
        int i;
        for (i = 5; i < strlen(input); ++i) {
            char c[2];
            c[0] = input[i];
            c[1] = 0;
            kprint(c);
        }
        kprint("\n");
    } else if (strcmp(input, "help") == 0) {
        kprint("Welcome to PawsOS, these are all the commands:\n"
            "\"end\": to halt the CPU.\n"
            "\"page\": to request a kmalloc().\n"
            "\"clear\": to clear the screen.\n"
            "\"cpuinfo\": to get information about your CPU.\n"
            "\"rtc\" to get the current time (in UTC).\n"
            "\"reboot\": to reboot your computer.\n"
            "\"echo [argument]\": to print something on the screen.\n"
            "\"shutdownvm\": to shutdown if you're using a VM.\n"
            "(There's no shutdown yet for real machines)\n");
    }

    if (dis_print == 0) {
        kprintf("You said: %s\n> ", input);
    } else if (dis_print == 1) {
        kprint("> ");
    } else {
        PANIC("No valid value on dis_print variable.", "shell.c", 11);
    }
}
