#include "string.h"
#include <stdint.h>
#include "ctype.h"

/**
 * K&R implementation
 */
void int_to_ascii(int n, char str[]) {
    int i, sign;
    if ((sign = n) < 0) n = -n;
    i = 0;
    do {
        str[i++] = n % 10 + '0';
    } while ((n /= 10) > 0);

    if (sign < 0) str[i++] = '-';
    str[i] = '\0';

    reverse(str);
}

void hex_to_ascii(int n, char str[]) {
    append(str, '0');
    append(str, 'x');
    char zeros = 0;

    int32_t tmp;
    int i;
    for (i = 28; i >= 0; i -= 4) {
        tmp = (n >> i) & 0xF;
        if (tmp == 0 && zeros == 0) continue;
        zeros = 1;
        if (tmp >= 0xA) append(str, tmp - 0xA + 'a');
        else append(str, tmp + '0');
    }
}

void hex_to_ascii_upper(int n, char str[]) {
    append(str, '0');
    append(str, 'x');
    char zeros = 0;

    int32_t tmp;
    int i;
    for (i = 28; i >= 0; i -= 4) {
        tmp = (n >> i) & 0xF;
        if (tmp == 0 && zeros == 0) continue;
        zeros = 1;
        if (tmp >= 0xA) append(str, tmp - 0xA + 'A');
        else append(str, tmp + '0');
    }
}

char *octal_to_ascii(int n) {
    static char representation[] = "01234567";
    static char buffer[50];
    char *ptr;

    ptr = &buffer[49];
    *ptr = '\0';

    do {
        *--ptr = representation[n % 8];
        n /= 8;
    } while (n != 0);

    return (ptr);
}

int ascii_to_int(const char *str) {
    int value = 0;

    while (isdigit(*str)) {
        value *= 10;
        value += (*str) - '0';
        str++;
    }

    return value;
}

int startswith(char *str, char *accept) {
    size_t s = strlen(accept);

    for (size_t i = 0; i < s; ++i) {
        if (*str != *accept) return 0;
        str++;
        accept++;
    }

    return 1;
}

// K&R
void reverse(char s[]) {
    int c, i, j;

    for (i = 0, j = strlen(s)-1; i < j; i++, j--) {
        c = s[i];
        s[i] = s[j];
        s[j] = c;
    }
}

int max(int a, int b) {
    return (a > b) ? a : b;
}

int min(int a, int b) {
    return (a > b) ? b : a;
}

void swap(int a, int b) {
    int t = a;
    a = b;
    b = t;
}

void append(char s[], char n) {
    int len = strlen(s);
    s[len] = n;
    s[len+1] = '\0';
}

void backspace(char s[]) {
    int len = strlen(s);
    s[len-1] = '\0';
}

/* K&R 
 * Returns <0 if s1<s2, 0 if s1==s2, >0 if s1>s2 */
int strcmp(const char s1[], const char s2[]) {
    int i;

    for (i = 0; s1[i] == s2[i]; i++) {
        if (s1[i] == '\0') return 0;
    }

    return s1[i] - s2[i];
}

int strncmp(const char s1[], const char s2[], size_t n) {
    int i;

    for (i = 0; n && s1[i] == s2[i]; ++i, --n) {
        if (s1[i] == '\0') return 0;
    }

    return (s1[i] - s2[i]);
}

// K&R
size_t strlen(const char s[]) {
    int i = 0;

    while (s[i] != '\0') {
        ++i;
    }

    return i;
}

char *strcpy(char *dest, const char *src) {
    char c;
    char *ptr = dest;

    while ((c = *src++)) {
        *ptr++ = c;
    }

    *ptr = '\0';
    return dest;
}

char *strncpy(char *dest, const char *src, size_t n) {
    char *ptr = dest;

    while (*src != '\0' && n--) {
        *ptr++ = *src++;
    }

    *ptr = '\0';
    return dest;
}

char *strcat(char *dest, const char *src) {
    char *ptr = dest + strlen(dest);

    while (*src != '\0') {
        *ptr++ = *src++;
    }

    *ptr = '\0';
    return dest;
}

char *strncat(char *dest, const char *src, size_t n) {
    char *ptr = dest + strlen(dest);

    while (*src != '\0' && n--) {
        *ptr++ = *src++;
    }

    *ptr = '\0';
    return dest;
}

char *strtruncate(char *str, int n) {
    if (n <= 0) return str;
    int l = n;
    int len = strlen(str);
    if (n > len) l = len;
    str[len-l] = '\0';
    return str;
}
