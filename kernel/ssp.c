#include "../libc/string.h"
#include "panic.h"

#define STACK_CHK_GUARD 0xE2DEE396

uintptr_t __stack_chk_guard = STACK_CHK_GUARD;

void __stack_chk_fail() {
    PANIC(__FILE__, __LINE__, "Stack smash detected.");
}
