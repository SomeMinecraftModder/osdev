#ifndef PRINTF_H
#define PRINTF_H

#include <stdarg.h>
#include <stddef.h>

#ifdef __GNUC__
int vsnprintf(char *buffer, size_t count, const char *format, va_list va)
  __attribute__((format(printf, 3, 0)));
int snprintf(char *buffer, size_t count, const char *format, ...)
  __attribute__((format(printf, 3, 4)));
int vasprintf(char **buffer, const char *format, va_list va)
  __attribute__((format(printf, 2, 0)));
int vsprintf(char *buffer, const char *format, va_list va)
  __attribute__((format(printf, 2, 0)));
int asprintf(char **buffer, const char *format, ...)
  __attribute__((format(printf, 2, 3)));
int sprintf(char *buffer, const char *format, ...)
  __attribute__((format(printf, 2, 3)));
int vprintf(const char *format, va_list va)
  __attribute__((format(printf, 1, 0)));
int printf(const char *format, ...) __attribute__((format(printf, 1, 2)));
#else
int vsnprintf(char *buffer, size_t count, const char *format, va_list va);
int snprintf(char *buffer, size_t count, const char *format, ...);
int vasprintf(char **buffer, const char *format, va_list va);
int vsprintf(char *buffer, const char *format, va_list va);
int asprintf(char **buffer, const char *format, ...);
int sprintf(char *buffer, const char *format, ...);
int vprintf(const char *format, va_list va);
int printf(const char *format, ...);
#endif

#endif
