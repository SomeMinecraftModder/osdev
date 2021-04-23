#include "../drivers/screen.h"
#include "../libc/string.h"
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

    while ((c = *format++) != 0) {
        if (c == '%') {
            c = *format++;
            switch (c) {
                case 'o':
                    if (str) {
                        int number = (int) va_arg(arg, int);
                        char *t = octal_to_ascii(number);
                        strcpy(str + (*pos), t);
                        *pos = *pos + strlen(t);
                    } else {
                        int number = (int) va_arg(arg, int);
                        char *t = octal_to_ascii(number);
                        while (*t) {
                            putchar(*t);
                            t++;
                        }
                    }
                    break;

                case 'x':
                    if (str) {
                        char t[16] = "";
                        uint32_t number = (uint32_t) va_arg(arg, uint32_t);
                        hex_to_ascii(number, t);
                        strcpy(str + (*pos), t);
                        *pos = *pos + strlen(t);
                    } else {
                        char t[16] = "";
                        uint32_t number = (uint32_t) va_arg(arg, uint32_t);
                        hex_to_ascii(number, t);
                        int i = 0;
                        while (t[i] != 0) {
                            putchar(t[i++]);
                        }
                    }
                    break;

                case 'X':
                    if (str) {
                        char t[16] = "";
                        uint32_t number = (uint32_t) va_arg(arg, uint32_t);
                        hex_to_ascii_upper(number, t);
                        strcpy(str + (*pos), t);
                        *pos = *pos + strlen(t);
                    } else {
                        char t[16] = "";
                        uint32_t number = (uint32_t) va_arg(arg, uint32_t);
                        hex_to_ascii_upper(number, t);
                        int i = 0;
                        while (t[i] != 0) {
                            putchar(t[i++]);
                        }
                    }
                    break;

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

                case 'd':
                case 'i':
                    if (str) {
                        static char *t;
                        int number = (int) va_arg(arg, int);
                        int_to_ascii(number, t);
                        strcpy(str + (*pos), t);
                        *pos = *pos + strlen(t);
                    } else {
                        static char *t;
                        int number = (int) va_arg(arg, int);
                        int_to_ascii(number, t);
                        while (*t) {
                            putchar(*t);
                            t++;
                        }
                    }
                    break;

                default:
                    putchar(c);
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

void sprintf(char *buf, char *fmt, ...) {
    va_list ap;
    va_start(ap, fmt);
    vsprintf(buf, NULL, fmt, ap);
    va_end(ap);
}
