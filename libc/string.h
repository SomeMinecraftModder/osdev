#ifndef STRINGS_H
#define STRINGS_H

#include <stddef.h>

int strncmp(const char s1[], const char s2[], size_t n);
char *strncpy(char *dest, const char *src, size_t n);
char *strncat(char *dest, const char *src, size_t n);
int strcmp(const char s1[], const char s2[]);
char *strcpy(char *dest, const char *src);
char *strcat(char *dest, const char *src);
char *strtruncate(char *str, int n);
size_t strlen(const char s[]);

void hex_to_ascii_upper(int n, char str[]);
void int_to_ascii(int n, char str[]);
void hex_to_ascii(int n, char str[]);
int ascii_to_int(const char *str);
char *octal_to_ascii(int n);

int startswith(char *str, char *accept);
void append(char s[], char n);
void backspace(char s[]);
void swap(int a, int b);
void reverse(char s[]);
int max(int a, int b);
int min(int a, int b);

#endif
