#include <stdbool.h>
#include "string.h"
#include <stdint.h>

bool isdigit(char c) {
    return c >= '0' && c <= '9';
}

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

int ascii_to_int(char *str) {
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
int strcmp(char s1[], char s2[]) {
    int i;
    for (i = 0; s1[i] == s2[i]; i++) {
        if (s1[i] == '\0') return 0;
    }
    return s1[i] - s2[i];
}

int strncmp(char s1[], char s2[], int n) {
    int i;
    for (i = 0; n && s1[i] == s2[i]; ++i, --n) {
        if (s1[i] == '\0') return 0;
    }
    return (s1[i] - s2[i]);
}

// K&R
int strlen(char s[]) {
    int i = 0;
    while (s[i] != '\0') {
        ++i;
    }
    return i;
}

char *strcpy(char *src, char *dst) {
    char *ptr = dst;
    while (*src != '\0') {
        *ptr++ = *src++;
    }
    *ptr = '\0';
    return dst;
}

char *strncpy(char *src, char *dst, int n) {
    char *ptr = dst;
    while (*src != '\0' && n--) {
        *ptr++ = *src++;
    }
    *ptr = '\0';
    return dst;
}

char *strcat(char *dst, char *src) {
    char *ptr = dst + strlen(dst);
    while (*src != '\0') {
        *ptr++ = *src++;
    }
    *ptr = '\0';
    return dst;
}

char *strncat(char *dst, char *src, int n) {
    char *ptr = dst + strlen(dst);
    while (*src != '\0' && n--) {
        *ptr++ = *src++;
    }
    *ptr = '\0';
    return dst;
}

char *strtruncate(char *str, int n) {
    if (n <= 0) return str;
    int l = n;
    int len = strlen(str);
    if (n > len) l = len;
    str[len-l] = '\0';
    return str;
}

int memcmp(void *s1, void *s2, size_t n) {
    uint8_t *byte1 = (uint8_t *)s1;
    uint8_t *byte2 = (uint8_t *)s2;
    while ((*byte1 == *byte2) && (n > 0)) {
        ++byte1;
        ++byte2;
        --n;
    }

    if (n == 0) {
        return 0;
    }
    return *byte1 - *byte2;
}
