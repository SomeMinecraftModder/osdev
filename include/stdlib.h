#ifndef STDLIB_H
#define STDLIB_H

#include <stddef.h>

#ifdef EXIT_FAILURE
    #undef EXIT_FAILURE
#endif

#define EXIT_FAILURE 1

#ifdef EXIT_SUCCESS
    #undef EXIT_SUCCESS
#endif

#define EXIT_SUCCESS 0

#ifdef RAND_MAX
    #undef RAND_MAX
#endif

#define RAND_MAX (0x7FFFFFFF)

#ifdef MB_CUR_MAX
    #undef MB_CUR_MAX
#endif

#define MB_CUR_MAX 1

typedef struct {
    int quot, rem;
} div_t;

typedef struct {
    long quot, rem;
} ldiv_t;

typedef struct {
    long long quot, rem;
} lldiv_t;

void *bsearch(const void *key, const void *base0, size_t nmemb, size_t size,
              int (*compar)(const void *, const void *));
void qsort(void *base, size_t nel, size_t width,
           int (*cmp)(const void *, const void *));
unsigned long long strtoull(const char *nptr, char **endptr, register int base);
unsigned long strtoul(const char *nptr, char **endptr, register int base);
size_t mbstowcs(wchar_t *restrict ws, const char *restrict s, size_t wn);
size_t wcstombs(char *restrict s, const wchar_t *restrict ws, size_t n);
long long strtoll(const char *nptr, char **endptr, register int base);
int mbtowc(wchar_t *restrict wc, const char *restrict src, size_t n);
long strtol(const char *nptr, char **endptr, int base);
void *aligned_alloc(size_t alignment, size_t size);
long double strtold(char *str, char **endptr);
lldiv_t lldiv(long long num, long long den);
int at_quick_exit(void (*function)(void));
float strtof(char *str, char **endptr);
char *itoa(int n, char *str, int base);
void *calloc(size_t num, size_t size);
void *realloc(void *ptr, size_t size);
_Noreturn void quick_exit(int status);
double strtod(char *str, char **ptr);
int mblen(const char *s, size_t n);
int atexit(void (*function)(void));
long long atoll(const char *str);
_Noreturn void _Exit(int status);
_Noreturn void exit(int status);
int wctomb(char *s, wchar_t wc);
int system(const char *command);
ldiv_t ldiv(long num, long den);
char *getenv(const char *name);
void srand(unsigned int seed);
long long llabs(long long a);
div_t div(int num, int den);
long atol(const char *str);
double atof(const char *s);
int atoi(const char *str);
void *malloc(size_t size);
_Noreturn void abort();
void free(void *p);
long labs(long a);
int abs(int a);
int rand();

#endif
