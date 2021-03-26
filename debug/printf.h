#ifndef PRINTF_H
#define PRINTF_H

#include <stdint.h>
#include <stdarg.h>

int is_format_letter(char c);

void vsprintf(char *str, void (*putchar)(char), char *format, va_list arg);

void vsprintf_helper(char *str, void (*putchar)(char), char *format, uint32_t *pos, va_list arg);

void kprintf(char *s, ...);

#endif
