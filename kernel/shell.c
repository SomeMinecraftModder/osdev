#include "../drivers/screen.h"
#include "../drivers/reboot.h"
#include "../drivers/acpi.h"
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
        kprintf("Page: %X, physical address: %X\n", page, phys_addr);
    } else if (strcmp(input, "clear") == 0) {
        clear_screen();
        dis_print = 1;
    } else if (strcmp(input, "reboot") == 0) {
        reboot();
    } else if (strcmp(input, "shutdown") == 0) {
        acpi_shutdown();
    } else if (strcmp(input, "cpuinfo") == 0) {
        cpudetect();
    } else if (strcmp(input, "rtc") == 0) {
        rtctime();
    } else if (strncmp(input, "echo ", 4) == 0) {
        size_t i;
        for (i = 5; i < strlen(input); ++i) {
            putchar(input[i]);
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
            "\"shutdown\": to shutdown your computer.\n"
            "Press Tab to enable AZERTY keyboard layout.\n"
            "Press Left Shift to print last used command.\n");
    }

    if (!dis_print) {
        kprintf("You said: %s\n> ", input);
    } else if (dis_print) {
        kprint("> ");
    } else {
        PANIC("No Valid Value in \"dis_print\" Variable.", "shell.c", 13);
    }
}
