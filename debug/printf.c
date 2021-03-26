#include "../drivers/screen.h"
#include "../libc/string.h"
#include "../libc/mem.h"
#include "printf.h"
#include <stddef.h>

int is_format_letter(char c) {
    return c == 'c' || c == 'd' || c == 'i' || c == 'e' || c == 'E' || c == 'f' || c == 'g' || c == 'G' || c == 'o' || c == 's' || c == 'u' || c == 'x' || c == 'X' || c == 'p' || c == 'n';
}

void vsprintf(char *str, void (*putchar)(char), char *format, va_list arg) {
    uint32_t pos = 0;
    vsprintf_helper(str, putchar, format, &pos, arg);
}

void vsprintf_helper(char *str, void (*putchar)(char), char *format, uint32_t *pos, va_list arg) {
    char c;
    uint8_t buf[512];
    memory_set(buf, 0, 512);

    while ((c = *format++) != 0) {
        if (c == '%') {
            c = *format++;
            switch (c) {
                // Handle calls like printf("%08x", 0xaa);
                case '0':
                    // Get the number between 0 and (x/d/p...)
                    c = *format;
                    while (!is_format_letter(c)) {
                        format++;
                        c = *format;
                    }
                    format++;
                    break;
                case 'd':
                case 'u':
                case 'x':
                case 'c':
                    if (str) {
                        *(str + *pos) = (char)va_arg(arg, int);
                        *pos = *pos + 1;
                    } else {
                        (*putchar)((char)va_arg(arg, int));
                    }
                    break;
                case 's':
                    if (str) {
                        char *t = (char *) va_arg(arg, int);
                        strcpy(str + (*pos), t);
                        *pos = *pos + strlen(t);
                    } else {
                        char *t = (char *) va_arg(arg, int);
                        while (*t) {
                            putchar(*t);
                            t++;
                        }
                    }
                    break;
                case 'i':
                    if (str) {
                        static char *t;
                        int nu = (int) va_arg(arg, int);
                        int_to_ascii(nu, t);
                        strcpy(str + (*pos), t);
                        *pos = *pos + strlen(t);
                    } else {
                        static char *t;
                        int nu = (int) va_arg(arg, int);
                        int_to_ascii(nu, t);
                        while (*t) {
                            putchar(*t);
                            t++;
                        }
                    }
                default:
                    break;
            }
            continue;
        }
        if (str) {
            *(str + *pos) = c;
            *pos = *pos + 1;
        } else {
            (*putchar)(c);
        }
    }
}

void kprintf(char *s, ...) {
    va_list ap;
    va_start(ap, s);
    vsprintf(NULL, putchar, s, ap);
    va_end(ap);
}
