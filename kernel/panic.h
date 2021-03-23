#ifndef PANIC_H
#define PANIC_H

#include <stdint.h>

void PANIC(char *PANIC_MSG, char *file, uint32_t line);
extern int halt();

#endif
