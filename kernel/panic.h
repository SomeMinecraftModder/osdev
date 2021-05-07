#ifndef PANIC_H
#define PANIC_H

#include <stdarg.h>
#include <stdint.h>

void PANIC(char *file, uint32_t line, char *PANIC_MSG, ...);
extern int halt();

#endif
