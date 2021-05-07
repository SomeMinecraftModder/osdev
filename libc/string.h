#ifndef STRINGS_H
#define STRINGS_H

#include <stddef.h>
#include <stdint.h>

size_t strxfrm(char *restrict dest, const char *restrict src, size_t n);
char *strncat(char *restrict dest, const char *restrict src, size_t n);
char *strncpy(char *restrict dest, const char *restrict src, size_t n);
char *stpncpy(char *restrict d, const char *restrict s, size_t n);
char *strcat(char *restrict dest, const char *restrict src);
char *strcpy(char *restrict dest, const char *restrict src);
char *strtok(char *restrict s, const char *restrict sep);
int strncmp(const char s1[], const char s2[], size_t n);
char *stpcpy(char *restrict d, const char *restrict s);
size_t strlcpy(char *d, const char *s, size_t n);
int strcmp(const char s1[], const char s2[]);
size_t strcspn(const char *s, const char *c);
size_t strspn(const char *s, const char *c);
char *strpbrk(const char *s, const char *b);
char *strrstr(const char *h, const char *n);
char *strstr(const char *h, const char *n);
int strcoll(const char *l, const char *r);
char *strsep(char **str, const char *sep);
size_t strnlen(const char *s, int32_t n);
char *strndup(const char *s, size_t n);
char *strchrnul(const char *s, int c);
char *strtruncate(char *str, int n);
char *strrchr(const char *s, int c);
char *strchr(const char *s, int c);
size_t strlen(const char s[]);
char *strdup(const char *s);

void *memccpy(void *restrict dest, const void *restrict src, int c, size_t n);
void *memmove(void *dest, const void *src, size_t nbytes);
void *mempcpy(void *dest, const void *src, size_t nbytes);
void *memcpy(void *dest, const void *src, size_t nbytes);
int memcmp(const void *s1, const void *s2, size_t n);
void *memset(void *dest, uint8_t val, size_t len);
void *memchr(const void *buf, int c, size_t n);
void *memrchr(const void *m, int c, size_t n);

char *alt_hex_to_ascii_upper(int n, char str[]);
char *hex_to_ascii_upper(int n, char str[]);
char *alt_hex_to_ascii(int n, char str[]);
char *hex_to_ascii(int n, char str[]);
char *int_to_ascii(int n, char str[]);
char *octal_to_ascii(int n);
int atoi(const char *str);

int startswith(char *str, char *accept);
void append(char s[], char n);
void backspace(char s[]);
void swap(int a, int b);
char *reverse(char s[]);

#endif
