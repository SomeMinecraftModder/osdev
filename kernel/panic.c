#include "../drivers/screen.h"
#include "../debug/printf.h"
#include "panic.h"

void PANIC(char *PANIC_MSG, char *file, uint32_t line) {
    clear_screen();
    kprint("[ ");
    kprint_at("PANIC", -1, -1, 0x04);
    kprintf(" ]:\n%s At %s:%i", PANIC_MSG, file, line);
    halt();
}
