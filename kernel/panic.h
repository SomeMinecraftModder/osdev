#ifndef PANIC_H
#define PANIC_H

#include <stdint.h>
#include <stdarg.h>

void PANIC(char *file, uint32_t line, char *PANIC_MSG, ...);
extern int halt();

#endif
