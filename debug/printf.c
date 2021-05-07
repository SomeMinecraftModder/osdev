#include "printf.h"
#include "../drivers/screen.h"
#include "../libc/string.h"
#include <stddef.h>

int zeroval = 0;
int zeropad = 0;
int altform = 0;

int is_format_letter(char c) {
    return c == 'c' || c == 'd' || c == 'i' || c == 'e' || c == 'E' ||
           c == 'f' || c == 'g' || c == 'G' || c == 'o' || c == 's' ||
           c == 'u' || c == 'x' || c == 'X' || c == 'p' || c == 'n';
}

static char *strcon(char *str, int zeropad, int num) {
    int padding = num - (int)strlen(str);
    if (padding < 0) {
        padding = 0;
    }

    char a[256] = {0};

    while (padding--) {
        if (zeropad) {
            a[strlen(a)] = '0';
        }
    }
    strcat(a, str);
    strcpy(str, a);

    zeropad = 0;
    zeroval = 0;

    return str;
}

void vsprintf(char *str, void (*putchar)(char), char *format, va_list arg) {
    uint32_t pos = 0;
    vsprintf_helper(str, putchar, format, &pos, arg);
}

void vsprintf_helper(char *str, void (*putchar)(char), char *format,
                     uint32_t *pos, va_list arg) {
    char c;

    while ((c = *format++) != 0) {
        if (c == '%') {
            c = *format++;
            if (c == '#') {
                altform = 1;
                c = *format++;
            }

            if (c == '0') {
                int i = 0;
                char pc[10];
                zeropad = 1;
                c = *format;
                while (!is_format_letter(c)) {
                    pc[i++] = c;
                    format++;
                    c = *format;
                }
                pc[i] = 0;
                c = *format++;
                zeroval = atoi(pc);
            }

            switch (c) {
                case 'o':
                    if (str) {
                        uint32_t number = (uint32_t)va_arg(arg, int);
                        char *t = octal_to_ascii(number);
                        t = strcon(t, zeropad, zeroval);
                        strcpy(str + (*pos), t);
                        *pos = *pos + strlen(t);
                    } else {
                        uint32_t number = (uint32_t)va_arg(arg, int);
                        char *t = octal_to_ascii(number);
                        t = strcon(t, zeropad, zeroval);
                        while (*t) {
                            putchar(*t);
                            t++;
                        }
                    }
                    break;

                case 'x':
                    if (str) {
                        char num[256] = "";
                        uint32_t number = (uint32_t)va_arg(arg, int);
                        if (altform == 0) {
                            hex_to_ascii(number, num);
                        } else {
                            alt_hex_to_ascii(number, num);
                        }
                        strcpy(str + (*pos), num);
                        *pos = *pos + strlen(num);
                    } else {
                        char num[256] = "";
                        uint32_t number = (uint32_t)va_arg(arg, int);
                        if (altform == 0) {
                            hex_to_ascii(number, num);
                        } else {
                            alt_hex_to_ascii(number, num);
                        }
                        int i = 0;
                        while (num[i]) {
                            putchar(num[i++]);
                        }
                    }
                    break;

                case 'X':
                    if (str) {
                        char num[256] = "";
                        int number = va_arg(arg, int);
                        if (altform == 0) {
                            hex_to_ascii_upper(number, num);
                        } else {
                            alt_hex_to_ascii_upper(number, num);
                        }
                        strcpy(str + (*pos), num);
                        *pos = *pos + strlen(num);
                    } else {
                        char num[256] = "";
                        int number = va_arg(arg, int);
                        if (altform == 0) {
                            hex_to_ascii_upper(number, num);
                        } else {
                            alt_hex_to_ascii_upper(number, num);
                        }
                        int i = 0;
                        while (num[i]) {
                            putchar(num[i++]);
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
                        char *t = (char *)va_arg(arg, int);
                        strcpy(str + (*pos), t);
                        *pos = *pos + strlen(t);
                    } else {
                        char *t = (char *)va_arg(arg, int);
                        while (*t) {
                            putchar(*t);
                            t++;
                        }
                    }
                    break;

                case 'd':
                case 'i':
                    if (str) {
                        static char *num;
                        int number = va_arg(arg, int);
                        int_to_ascii(number, num);
                        num = strcon(num, zeropad, zeroval);
                        strcpy(str + (*pos), num);
                        *pos = *pos + strlen(num);
                    } else {
                        static char *num;
                        int number = va_arg(arg, int);
                        int_to_ascii(number, num);
                        num = strcon(num, zeropad, zeroval);
                        while (*num) {
                            putchar(*num);
                            num++;
                        }
                    }
                    break;

                case 'u':
                    if (str) {
                        static char *num;
                        uint32_t number = (uint32_t)va_arg(arg, int);
                        int_to_ascii(number, num);
                        num = strcon(num, zeropad, zeroval);
                        strcpy(str + (*pos), num);
                        *pos = *pos + strlen(num);
                    } else {
                        static char *num;
                        uint32_t number = (uint32_t)va_arg(arg, int);
                        int_to_ascii(number, num);
                        num = strcon(num, zeropad, zeroval);
                        while (*num) {
                            putchar(*num);
                            num++;
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
