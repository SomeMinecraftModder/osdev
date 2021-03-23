#include "../drivers/screen.h"
#include "../cpu/isr.h"
#include "panic.h"

void PANIC(char *PANIC_MSG, char *file, uint32_t line) {
    kprint("[ ");
    kprint_at("PANIC", -1, -1, 0x04);
    kprint(" ]:");
    kprint("\n");
    kprint(PANIC_MSG);
    kprint(" at ");
    kprint(file);
    kprint(" : ");
    kprint_dec(line);
    halt();
}
