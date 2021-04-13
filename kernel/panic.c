#include "../drivers/screen.h"
#include "../debug/printf.h"
#include <stddef.h>
#include "panic.h"

void PANIC(char *file, uint32_t line, char *PANIC_MSG, ...) {
    clear_screen();
    kprint("[ ");
    kprint_at("PANIC", -1, -1, 0x04);
    kprintf(" ]:\n");
    va_list ap;
    va_start(ap, PANIC_MSG);
    vsprintf(NULL, putchar, PANIC_MSG, ap);
    va_end(ap);
    kprintf(" At %s:%i", file, line);
    halt();
}
