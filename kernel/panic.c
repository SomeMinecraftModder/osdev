#include "../drivers/screen.h"
#include "../drivers/serial.h"
#include "../debug/printf.h"
#include "../libc/mem.h"
#include <stddef.h>
#include "panic.h"

extern char start, end;

void PANIC(char *file, uint32_t line, char *PANIC_MSG, ...) {
    clear_screen();
    kprint("[ ");
    kprint_color("PANIC", red);
    kprintf(" ]:\n");
    va_list ap;
    va_start(ap, PANIC_MSG);
    vsprintf(NULL, putchar, PANIC_MSG, ap);
    va_end(ap);
    kprintf(" At %s:%i", file, line);
    printf_serial("[ KERNEL PANIC! ]\n%s At %s:%i\n", PANIC_MSG, file, line);
    memdump(&start, &end);
    halt();
}
