#include "panic.h"
#include "../debug/printf.h"
#include "../drivers/screen.h"
#include "../drivers/serial.h"
#include <string.h>

_Noreturn void PANIC(char *file, uint32_t line, char *PANIC_MSG) {
    clear_screen(black);
    kprint("[ ");
    kprint_color("PANIC", red);
    kprint(" ]:\n");
    write_serial("[ PANIC ]:\n");
    printf(PANIC_MSG);
    write_serial(PANIC_MSG);
    printf(" At %s:%li", file, line);
    write_serial(" At ");
    write_serial(file);
    write_serial(":");
    char c[256];
    write_serial(int_to_ascii(line, c));
    halt();
}
