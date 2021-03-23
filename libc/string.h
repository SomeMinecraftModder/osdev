#ifndef STRINGS_H
#define STRINGS_H

int startswith(char *str, char *accept);
void int_to_ascii(int n, char str[]);
void hex_to_ascii(int n, char str[]);
int strcmp(char s1[], char s2[]);
void append(char s[], char n);
int ascii_to_int(char *str);
void swap(int a, int b);
void backspace(char s[]);
void reverse(char s[]);
int max(int a, int b);
int min(int a, int b);
int strlen(char s[]);

#endif
