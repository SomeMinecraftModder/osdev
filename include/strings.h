#ifndef _STRINGS_H
#define _STRINGS_H

#include <stddef.h>

int strncasecmp(const char *_l, const char *_r, size_t n);
int strcasecmp(const char *_l, const char *_r);

#if defined(_GNU_SOURCE) || defined(_BSD_SOURCE) || defined(_POSIX_SOURCE) || \
  (defined(_POSIX_C_SOURCE) && _POSIX_C_SOURCE < 200809L) ||                  \
  (defined(_XOPEN_SOURCE) && _XOPEN_SOURCE < 700)
int bcmp(const void *s1, const void *s2, size_t n);
void bcopy(const void *s1, void *s2, size_t n);
char *rindex(const char *s, int c);
char *index(const char *s, int c);
void bzero(void *s, size_t n);
#endif

#if defined(_XOPEN_SOURCE) || defined(_GNU_SOURCE) || defined(_BSD_SOURCE)
int ffsll(long long i);
int ffsl(long i);
int ffs(int i);
#endif

#endif
