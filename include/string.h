#ifndef _STRING_H
#define _STRING_H

#include <stddef.h>
#include <stdint.h>

size_t strxfrm(char *restrict dest, const char *restrict src, size_t n);
char *strncat(char *restrict dest, const char *restrict src, size_t n);
char *strncpy(char *restrict dest, const char *restrict src, size_t n);
char *strcat(char *restrict dest, const char *restrict src);
char *strcpy(char *restrict dest, const char *restrict src);
char *strtok(char *restrict s, const char *restrict sep);
int strncmp(const char s1[], const char s2[], size_t n);
int strcmp(const char s1[], const char s2[]);
size_t strcspn(const char *s, const char *c);
size_t strspn(const char *s, const char *c);
char *strpbrk(const char *s, const char *b);
char *strrstr(const char *h, const char *n);
char *strstr(const char *h, const char *n);
int strcoll(const char *l, const char *r);
char *strtruncate(char *str, int n);
char *strrchr(const char *s, int c);
char *strchr(const char *s, int c);
size_t strlen(const char s[]);
char *strerror(int e);

#if defined(_POSIX_SOURCE) || defined(_POSIX_C_SOURCE) || \
  defined(_XOPEN_SOURCE) || defined(_GNU_SOURCE) || defined(_BSD_SOURCE)
char *strtok_r(char *restrict s, const char *restrict sep, char **restrict p);
char *stpncpy(char *restrict d, const char *restrict s, size_t n);
char *stpcpy(char *restrict d, const char *restrict s);
int strerror_r(int err, char *buf, size_t buflen);
size_t strnlen(const char *s, int32_t n);
char *strndup(const char *s, size_t n);
char *strdup(const char *s);
char *strsignal(int signum);
#endif

#if defined(_XOPEN_SOURCE) || defined(_GNU_SOURCE) || defined(_BSD_SOURCE)
void *memccpy(void *restrict dest, const void *restrict src, int c, size_t n);
#endif

#if defined(_GNU_SOURCE) || defined(_BSD_SOURCE)
size_t strlcpy(char *d, const char *s, size_t n);
size_t strlcat(char *d, const char *s, size_t n);
char *strsep(char **str, const char *sep);
#endif

#ifdef _GNU_SOURCE
    #define strdupa(x) strcpy(alloca(strlen(x) + 1), x)
void *mempcpy(void *dest, const void *src, size_t n);
int strverscmp(const char *l0, const char *r0);
char *strcasestr(const char *h, const char *n);
void *memrchr(const void *m, int c, size_t n);
char *strchrnul(const char *s, int c);
#endif

void *memcpy(void *restrict dest, const void *restrict src, size_t n);
void *memmove(void *dest, const void *src, size_t n);
int memcmp(const void *vl, const void *vr, size_t n);
void *memchr(const void *src, int c, size_t n);
void *memset(void *dest, uint8_t c, size_t n);

char *alt_hex_to_ascii_upper(int n, char str[]);
char *hex_to_ascii_upper(int n, char str[]);
char *alt_hex_to_ascii(int n, char str[]);
char *hex_to_ascii(int n, char str[]);
char *int_to_ascii(int n, char str[]);
char *octal_to_ascii(int n);

int startswith(char *str, char *accept);
void append(char s[], char n);
void backspace(char s[]);
void swap(int a, int b);
char *reverse(char s[]);

#endif
