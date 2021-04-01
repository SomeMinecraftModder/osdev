#ifndef STRINGS_H
#define STRINGS_H

#include <stddef.h>

char *strncpy(char *src, char *dst, int n);
char *strncat(char *dst, char *src, int n);
void hex_to_ascii_upper(int n, char str[]);
int strncmp(char s1[], char s2[], int n);
int memcmp(void *s1, void *s2, size_t n);
int startswith(char *str, char *accept);
void int_to_ascii(int n, char str[]);
void hex_to_ascii(int n, char str[]);
char *strtruncate(char *str, int n);
char *strcpy(char *src, char *dst);
char *strcat(char *dst, char *src);
int strcmp(char s1[], char s2[]);
void append(char s[], char n);
char *octal_to_ascii(int n);
int ascii_to_int(char *str);
void backspace(char s[]);
void swap(int a, int b);
void reverse(char s[]);
int max(int a, int b);
int min(int a, int b);
int strlen(char s[]);

#endif
