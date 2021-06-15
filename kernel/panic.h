#ifndef PANIC_H
#define PANIC_H

#include <stdint.h>

_Noreturn void PANIC(char *file, uint32_t line, char *PANIC_MSG);
extern _Noreturn void halt();

#endif
