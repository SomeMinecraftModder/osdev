#include "panic.h"
#include "../debug/printf.h"
#include "../drivers/screen.h"
#include "../drivers/serial.h"
#include <stddef.h>

void PANIC(char *file, uint32_t line, char *PANIC_MSG, ...) {
    clear_screen(black);
    kprint("UU");
    kprint("[ ");
    kprint_color("PANIC", red);
    kprint(" ]:\n");
    write_serial("[ PANIC ]:\n");
    va_list ap;
    va_start(ap, PANIC_MSG);
    vsprintf(NULL, putchar, PANIC_MSG, ap);
    vsprintf(NULL, write_serial_char, PANIC_MSG, ap);
    va_end(ap);
    kprintf(" At %s:%i", file, line);
    printf_serial(" At %s:%i", file, line);
    halt();
}
