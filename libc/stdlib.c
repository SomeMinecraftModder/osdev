#include "../debug/printf.h"
#include "../drivers/screen.h"
#include "../kernel/shell.h"
#include "kheap.h"
#include "math.h"
#include <ctype.h>
#include <errno.h>
#include <limits.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#ifdef __STRICT_ANSI__
    #define asm __asm__
#endif

typedef struct __mbstate_t {
    unsigned __opaque1, __opaque2;
} mbstate_t;

int regedqhands = 0;
int regedhands = 0;

void (*atqexitfunc[256])(void);
void (*atexitfunc[256])(void);

#define OOB(c, b) \
    (((((b) >> 3) - 0x10) | (((b) >> 3) + ((int32_t)(c) >> 26))) & ~7)

#define R(a, b) ((uint32_t)((a == 0x80 ? 0x40u - b : 0u - a) << 23))

#define C(x) (x < 2 ? (unsigned)-1 : (R(0x80, 0xC0) | x))
#define D(x) C((x + 16))
#define E(x)                                                               \
    ((x == 0 ? R(0xA0, 0xC0) : x == 0xD ? R(0x80, 0xA0) : R(0x80, 0xC0)) | \
     (R(0x80, 0xC0) >> 6) | x)
#define F(x)                                                                 \
    ((x >= 5                                                                 \
        ? 0                                                                  \
        : x == 0 ? R(0x90, 0xC0) : x == 4 ? R(0x80, 0x90) : R(0x80, 0xC0)) | \
     (R(0x80, 0xC0) >> 6) | (R(0x80, 0xC0) >> 12) | x)

const uint32_t bittab[] = {
  C(0x2), C(0x3), C(0x4), C(0x5), C(0x6), C(0x7), C(0x8), C(0x9), C(0xA),
  C(0xB), C(0xC), C(0xD), C(0xE), C(0xF), D(0x0), D(0x1), D(0x2), D(0x3),
  D(0x4), D(0x5), D(0x6), D(0x7), D(0x8), D(0x9), D(0xA), D(0xB), D(0xC),
  D(0xD), D(0xE), D(0xF), E(0x0), E(0x1), E(0x2), E(0x3), E(0x4), E(0x5),
  E(0x6), E(0x7), E(0x8), E(0x9), E(0xA), E(0xB), E(0xC), E(0xD), E(0xE),
  E(0xF), F(0x0), F(0x1), F(0x2), F(0x3), F(0x4)};

#define SA 0xC2U
#define SB 0xF4U

#define CODEUNIT(c)    (0xDFFF & (signed char)(c))
#define IS_CODEUNIT(c) ((unsigned)(c)-0xDF80 < 0x80)

static uint32_t x = 123456789;
static uint32_t y = 362436069;
static uint32_t z = 521288629;
static uint32_t w = 88675123;

extern heap_t *kheap;

int atoi(const char *str) {
    int n = 0, neg = 0;
    while (isspace(*str)) {
        str++;
    }

    switch (*str) {
        case '-':
            neg = 1;
            break;

        case '+':
            str++;
            break;
    }

    while (isdigit(*str)) {
        n = 10 * n - (*str++ - '0');
    }

    return neg ? n : -n;
}

char *itoa(int n, char *str, int base) {
    if (base == 2) {
#pragma GCC diagnostic push // Don't warn about unknown conversion type
#pragma GCC diagnostic ignored "-Wformat="
#pragma GCC diagnostic ignored "-Wformat-extra-args"
        sprintf(str, "%b", n);
#pragma GCC diagnostic pop
    } else if (base == 8) {
        sprintf(str, "%o", n);
    } else if (base == 10) {
        sprintf(str, "%i", n);
    } else if (base == 16) {
        sprintf(str, "%X", n);
    }

    return str;
}

long atol(const char *str) {
    return strtol(str, (char **)NULL, 10);
}

long long atoll(const char *str) {
    return strtoll(str, (char **)NULL, 10);
}

double atof(const char *s) {
    double a = 0.0;
    int e = 0;
    int c;

    while ((c = *s++) != '\0' && isdigit(c)) {
        a = a * 10.0 + (c - '0');
    }

    if (c == '.') {
        while ((c = *s++) != '\0' && isdigit(c)) {
            a = a * 10.0 + (c - '0');
            e = e - 1;
        }
    }

    if (c == 'e' || c == 'E') {
        int sign = 1;
        int i = 0;
        c = *s++;

        if (c == '+') {
            c = *s++;
        } else if (c == '-') {
            c = *s++;
            sign = -1;
        }

        while (isdigit(c)) {
            i = i * 10 + (c - '0');
            c = *s++;
        }

        e += i * sign;
    }

    while (e > 0) {
        a *= 10.0;
        e--;
    }

    while (e < 0) {
        a *= 0.1;
        e++;
    }

    return a;
}

double strtod(char *str, char **ptr) {
    char *p;
    if (ptr == (char **)0) {
        return atof(str);
    }

    p = str;

    while (isspace(*p)) {
        ++p;
    }

    if (*p == '+' || *p == '-') {
        ++p;
    }

    if ((p[0] == 'i' || p[0] == 'I') && (p[1] == 'n' || p[1] == 'N') &&
        (p[2] == 'f' || p[2] == 'F')) {
        if ((p[3] == 'i' || p[3] == 'I') && (p[4] == 'n' || p[4] == 'N') &&
            (p[5] == 'i' || p[5] == 'I') && (p[6] == 't' || p[6] == 'T') &&
            (p[7] == 'y' || p[7] == 'Y')) {
            *ptr = p + 8;
            return atof(str);
        } else {
            *ptr = p + 3;
            return atof(str);
        }
    }

    if ((p[0] == 'n' || p[0] == 'N') && (p[1] == 'a' || p[1] == 'A') &&
        (p[2] == 'n' || p[2] == 'N')) {
        p += 3;
        if (*p == '(') {
            ++p;
            while (*p != '\0' && *p != ')') {
                ++p;
            }

            if (*p == ')') {
                ++p;
            }
        }

        *ptr = p;
        return atof(str);
    }

    if (isdigit(*p) || *p == '.') {
        int got_dot = 0;
        while (isdigit(*p) || (!got_dot && *p == '.')) {
            if (*p == '.')
                got_dot = 1;
            ++p;
        }

        if (*p == 'e' || *p == 'E') {
            int i;
            i = 1;
            if (p[i] == '+' || p[i] == '-') {
                ++i;
            }
            if (isdigit(p[i])) {
                while (isdigit(p[i])) {
                    ++i;
                }
                *ptr = p + i;
                return atof(str);
            }
        }
        *ptr = p;
        return atof(str);
    }

    *ptr = str;
    return 0.0;
}

float strtof(char *str, char **endptr) {
    return (float)strtod(str, endptr);
}

long double strtold(char *str, char **endptr) {
    return strtod(str, endptr);
}

long strtol(const char *nptr, char **endptr, int base) {
    const char *s;
    long acc, cutoff;
    int c;
    int neg, any, cutlim;
    s = nptr;

    do {
        c = (uint8_t)*s++;
    } while (isspace(c));

    if (c == '-') {
        neg = 1;
        c = *s++;
    } else {
        neg = 0;
        if (c == '+') {
            c = *s++;
        }
    }

    if ((base == 0 || base == 16) && c == '0' && (*s == 'x' || *s == 'X')) {
        c = s[1];
        s += 2;
        base = 16;
    }

    if (base == 0) {
        base = c == '0' ? 8 : 10;
    }

    cutoff = neg ? LONG_MIN : LONG_MAX;
    cutlim = cutoff % base;
    cutoff /= base;
    if (neg) {
        if (cutlim > 0) {
            cutlim -= base;
            cutoff += 1;
        }
        cutlim = -cutlim;
    }

    for (acc = 0, any = 0;; c = (uint8_t)*s++) {
        if (isdigit(c)) {
            c -= '0';
        } else if (isalpha(c)) {
            c -= isupper(c) ? 'A' - 10 : 'a' - 10;
        } else {
            break;
        }

        if (c >= base) {
            break;
        }

        if (any < 0) {
            continue;
        }

        if (neg) {
            if (acc < cutoff || (acc == cutoff && c > cutlim)) {
                any = -1;
                acc = LONG_MIN;
            } else {
                any = 1;
                acc *= base;
                acc -= c;
            }
        } else {
            if (acc > cutoff || (acc == cutoff && c > cutlim)) {
                any = -1;
                acc = LONG_MAX;
            } else {
                any = 1;
                acc *= base;
                acc += c;
            }
        }
    }

    if (endptr != 0) {
        *endptr = (char *)(any ? s - 1 : nptr);
    }

    return (acc);
}

long long strtoll(const char *nptr, char **endptr, register int base) {
    register const char *s = nptr;
    register uint64_t acc;
    register int c;
    register uint64_t cutoff;
    register int neg = 0, any, cutlim;

    do {
        c = *s++;
    } while (isspace(c));
    if (c == '-') {
        neg = 1;
        c = *s++;
    } else if (c == '+')
        c = *s++;
    if ((base == 0 || base == 16) && c == '0' && (*s == 'x' || *s == 'X')) {
        c = s[1];
        s += 2;
        base = 16;
    }

    if (base == 0) {
        base = c == '0' ? 8 : 10;
    }

    cutoff = neg ? -(uint64_t)LLONG_MIN : LLONG_MAX;
    cutlim = cutoff % (uint64_t)base;
    cutoff /= (uint64_t)base;
    for (acc = 0, any = 0;; c = *s++) {
        if (isdigit(c)) {
            c -= '0';
        } else if (isalpha(c)) {
            c -= isupper(c) ? 'A' - 10 : 'a' - 10;
        } else {
            break;
        }

        if (c >= base) {
            break;
        }

        if (any < 0 || acc > cutoff || (acc == cutoff && c > cutlim)) {
            any = -1;
        } else {
            any = 1;
            acc *= base;
            acc += c;
        }
    }

    if (any < 0) {
        acc = neg ? LLONG_MIN : LLONG_MAX;
    } else if (neg) {
        acc = -acc;
    }

    if (endptr != 0) {
        *endptr = (char *)(any ? s - 1 : nptr);
    }

    return (acc);
}

unsigned long strtoul(const char *nptr, char **endptr, register int base) {
    register const char *s = nptr;
    register uint32_t acc;
    register int c;
    register uint32_t cutoff;
    register int neg = 0, any, cutlim;

    do {
        c = *s++;
    } while (isspace(c));

    if (c == '-') {
        neg = 1;
        c = *s++;
    } else if (c == '+') {
        c = *s++;
    }

    if ((base == 0 || base == 16) && c == '0' && (*s == 'x' || *s == 'X')) {
        c = s[1];
        s += 2;
        base = 16;
    }

    if (base == 0) {
        base = c == '0' ? 8 : 10;
    }

    cutoff = (uint32_t)ULONG_MAX / (uint32_t)base;
    cutlim = (uint32_t)ULONG_MAX % (uint32_t)base;
    for (acc = 0, any = 0;; c = *s++) {
        if (isdigit(c)) {
            c -= '0';
        } else if (isalpha(c)) {
            c -= isupper(c) ? 'A' - 10 : 'a' - 10;
        } else {
            break;
        }

        if (c >= base) {
            break;
        }

        if (any < 0 || acc > cutoff || (acc == cutoff && c > cutlim)) {
            any = -1;
        } else {
            any = 1;
            acc *= base;
            acc += c;
        }
    }

    if (any < 0) {
        acc = ULONG_MAX;
        errno = ERANGE;
    } else if (neg) {
        acc = -acc;
    }

    if (endptr != 0) {
        *endptr = (char *)(any ? s - 1 : nptr);
    }

    return (acc);
}

unsigned long long strtoull(const char *nptr, char **endptr,
                            register int base) {
    register const char *s = nptr;
    register uint64_t acc;
    register int c;
    register uint64_t cutoff;
    register int neg = 0, any, cutlim;

    do {
        c = *s++;
    } while (isspace(c));
    if (c == '-') {
        neg = 1;
        c = *s++;
    } else if (c == '+')
        c = *s++;
    if ((base == 0 || base == 16) && c == '0' && (*s == 'x' || *s == 'X')) {
        c = s[1];
        s += 2;
        base = 16;
    }
    if (base == 0)
        base = c == '0' ? 8 : 10;
    cutoff = (uint64_t)ULLONG_MAX / (uint64_t)base;
    cutlim = (uint64_t)ULLONG_MAX % (uint64_t)base;
    for (acc = 0, any = 0;; c = *s++) {
        if (isdigit(c)) {
            c -= '0';
        } else if (isalpha(c)) {
            c -= isupper(c) ? 'A' - 10 : 'a' - 10;
        } else {
            break;
        }

        if (c >= base) {
            break;
        }

        if (any < 0 || acc > cutoff || (acc == cutoff && c > cutlim)) {
            any = -1;
        } else {
            any = 1;
            acc *= base;
            acc += c;
        }
    }

    if (any < 0) {
        acc = ULLONG_MAX;
        errno = ERANGE;
    } else if (neg) {
        acc = -acc;
    }

    if (endptr != 0) {
        *endptr = (char *)(any ? s - 1 : nptr);
    }

    return (acc);
}

int system(const char *command) {
    if (command == NULL) {
        return 1;
    } else {
        shell(command);
        return 0;
    }
}

void *bsearch(const void *key, const void *base0, size_t nmemb, size_t size,
              int (*compar)(const void *, const void *)) {
    const char *base = (const char *)base0;
    int lim, cmp;
    const void *p;

    for (lim = nmemb; lim != 0; lim >>= 1) {
        p = base + (lim >> 1) * size;
        cmp = (*compar)(key, p);
        if (cmp == 0)
            return (void *)p;
        if (cmp > 0) {
            base = (const char *)p + size;
            lim--;
        }
    }

    return (NULL);
}

static inline int a_ctz_64(uint64_t x) {
    int r;
    asm("bsf %1,%0 ; jnz 1f ; bsf %2,%0 ; add $32,%0\n1:"
        : "=&r"(r)
        : "r"((unsigned)x), "r"((unsigned)(x >> 32)));
    return r;
}

static inline int a_ctz_32(uint32_t x) {
    int r;
    asm("bsf %1,%0" : "=r"(r) : "r"(x));
    return r;
}

static inline int a_ctz_l(uint32_t x) {
    return (sizeof(long) < 8) ? a_ctz_32(x) : a_ctz_64(x);
}

static inline int pntz(size_t p[2]) {
    int r = a_ctz_l(p[0] - 1);

    if (r != 0 ||
        (r = 8 * sizeof(size_t) + a_ctz_l(p[1])) != 8 * sizeof(size_t)) {
        return r;
    }

    return 0;
}

static void cycle(size_t width, uint8_t *ar[], int n) {
    uint8_t tmp[256];
    size_t l;
    int i;

    if (n < 2) {
        return;
    }

    ar[n] = tmp;
    while (width) {
        l = sizeof(tmp) < width ? sizeof(tmp) : width;
        memcpy(ar[n], ar[0], l);
        for (i = 0; i < n; i++) {
            memcpy(ar[i], ar[i + 1], l);
            ar[i] += l;
        }
        width -= l;
    }
}

static inline void shl(size_t p[2], int n) {
    if ((unsigned)n >= 8 * sizeof(size_t)) {
        n -= 8 * sizeof(size_t);
        p[1] = p[0];
        p[0] = 0;
    }

    p[1] <<= n;
    p[1] |= p[0] >> (sizeof(size_t) * 8 - n);
    p[0] <<= n;
}

static inline void shr(size_t p[2], int n) {
    if ((unsigned)n >= 8 * sizeof(size_t)) {
        n -= 8 * sizeof(size_t);
        p[0] = p[1];
        p[1] = 0;
    }

    p[0] >>= n;
    p[0] |= p[1] << (sizeof(size_t) * 8 - n);
    p[1] >>= n;
}

static void sift(uint8_t *head, size_t width,
                 int (*cmp)(const void *, const void *), int pshift,
                 size_t lp[]) {
    uint8_t *rt, *lf;
    uint8_t *ar[14 * sizeof(size_t) + 1];
    int i = 1;

    ar[0] = head;
    while (pshift > 1) {
        rt = head - width;
        lf = head - width - lp[pshift - 2];

        if ((*cmp)(ar[0], lf) >= 0 && (*cmp)(ar[0], rt) >= 0) {
            break;
        }
        if ((*cmp)(lf, rt) >= 0) {
            ar[i++] = lf;
            head = lf;
            pshift -= 1;
        } else {
            ar[i++] = rt;
            head = rt;
            pshift -= 2;
        }
    }
    cycle(width, ar, i);
}

static void trinkle(unsigned char *head, size_t width,
                    int (*cmp)(const void *, const void *), size_t pp[2],
                    int pshift, int trusty, size_t lp[]) {
    uint8_t *stepson, *rt, *lf;
    size_t p[2];
    uint8_t *ar[14 * sizeof(size_t) + 1];
    int i = 1;
    int trail;

    p[0] = pp[0];
    p[1] = pp[1];

    ar[0] = head;
    while (p[0] != 1 || p[1] != 0) {
        stepson = head - lp[pshift];
        if ((*cmp)(stepson, ar[0]) <= 0) {
            break;
        }

        if (!trusty && pshift > 1) {
            rt = head - width;
            lf = head - width - lp[pshift - 2];
            if ((*cmp)(rt, stepson) >= 0 || (*cmp)(lf, stepson) >= 0) {
                break;
            }
        }

        ar[i++] = stepson;
        head = stepson;
        trail = pntz(p);
        shr(p, trail);
        pshift += trail;
        trusty = 0;
    }

    if (!trusty) {
        cycle(width, ar, i);
        sift(head, width, cmp, pshift, lp);
    }
}

void qsort(void *base, size_t nel, size_t width,
           int (*cmp)(const void *, const void *)) {
    size_t lp[12 * sizeof(size_t)];
    size_t i, size = width * nel;
    uint8_t *head, *high;
    size_t p[2] = {1, 0};
    int pshift = 1;
    int trail;

    if (!size) {
        return;
    }

    head = base;
    high = head + size - width;

    for (lp[0] = lp[1] = width, i = 2;
         (lp[i] = lp[i - 2] + lp[i - 1] + width) < size; i++)
        ;

    while (head < high) {
        if ((p[0] & 3) == 3) {
            sift(head, width, cmp, pshift, lp);
            shr(p, 2);
            pshift += 2;
        } else {
            if ((signed)lp[pshift - 1] >= high - head) {
                trinkle(head, width, cmp, p, pshift, 0, lp);
            } else {
                sift(head, width, cmp, pshift, lp);
            }

            if (pshift == 1) {
                shl(p, 1);
                pshift = 0;
            } else {
                shl(p, pshift - 1);
                pshift = 1;
            }
        }

        p[0] |= 1;
        head += width;
    }

    trinkle(head, width, cmp, p, pshift, 0, lp);

    while (pshift != 1 || p[0] != 1 || p[1] != 0) {
        if (pshift <= 1) {
            trail = pntz(p);
            shr(p, trail);
            pshift += trail;
        } else {
            shl(p, 2);
            pshift -= 2;
            p[0] ^= 7;
            shr(p, 1);
            trinkle(head - lp[pshift] - width, width, cmp, p, pshift + 1, 1,
                    lp);
            shl(p, 1);
            p[0] |= 1;
            trinkle(head - width, width, cmp, p, pshift, 1, lp);
        }

        head -= width;
    }
}

int abs(int a) {
    if (a < 0) {
        return -a;
    }

    return a;
}

long labs(long a) {
    if (a < 0) {
        return -a;
    }

    return a;
}

long long llabs(long long a) {
    if (a < 0) {
        return -a;
    }

    return a;
}

div_t div(int num, int den) {
    return (div_t){num / den, num % den};
}

ldiv_t ldiv(long num, long den) {
    return (ldiv_t){num / den, num % den};
}

lldiv_t lldiv(long long num, long long den) {
    return (lldiv_t){num / den, num % den};
}

int rand() {
    uint32_t t;

    t = x ^ (x << 11);
    x = y;
    y = z;
    z = w;
    return abs(w = w ^ (w >> 19) ^ t ^ (t >> 8));
}

void srand(unsigned int seed) {
    w ^= seed;
}

void *malloc(size_t size) {
    return (void *)kmalloc_int(size, 0, 0);
}

void free(void *p) {
    kfree(p, kheap);
}

void *calloc(size_t num, size_t size) {
    void *ptr = malloc(num * size);
    memset(ptr, 0, num * size);
    return ptr;
}

void *realloc(void *ptr, size_t size) {
    void *new_block;
    size_t old_size;

    new_block = malloc(size);
    if (new_block == NULL) {
        return NULL;
    }

    if (ptr == NULL) {
        old_size = 0;
    } else {
        header_t *header = (header_t *)((uint32_t)ptr - sizeof(header_t));
        old_size = header->size - sizeof(header_t) - sizeof(footer_t);
    }

    memcpy(new_block, ptr, min(old_size, size));
    free(ptr);
    return new_block;
}

void *aligned_alloc(size_t alignment, size_t size) {
    // This implementation does the work, I think
    return (void *)kmalloc_ap(alignment, malloc(size));
}

int mbtowc(wchar_t *restrict wc, const char *restrict src, size_t n) {
    unsigned c;
    const uint8_t *s = (const void *)src;
    wchar_t dummy;

    if (!s) {
        return 0;
    }

    if (!n) {
        goto ilseq;
    }

    if (!wc) {
        wc = &dummy;
    }

    if (*s < 0x80) {
        return !!(*wc = *s);
    }

    if (MB_CUR_MAX == 1) {
        return (*wc = CODEUNIT(*s)), 1;
    }

    if (*s - SA > SB - SA) {
        goto ilseq;
    }

    c = bittab[*s++ - SA];

    if (n < 4 && ((c << (6 * n - 6)) & (1U << 31))) {
        goto ilseq;
    }

    if (OOB(c, *s)) {
        goto ilseq;
    }

    c = (c << 6) | (*s++ - 0x80);
    if (!(c & (1U << 31))) {
        *wc = c;
        return 2;
    }

    if (*s - 0x80u >= 0x40) {
        goto ilseq;
    }

    c = (c << 6) | (*s++ - 0x80);
    if (!(c & (1U << 31))) {
        *wc = c;
        return 3;
    }

    if (*s - 0x80u >= 0x40) {
        goto ilseq;
    }

    *wc = (c << 6) | (*s++ - 0x80);
    return 4;

ilseq:
    errno = EILSEQ;
    return -1;
}

int mblen(const char *s, size_t n) {
    return mbtowc(0, s, n);
}

size_t mbsrtowcs(wchar_t *restrict ws, const char **restrict src, size_t wn,
                 mbstate_t *restrict st) {
    const uint8_t *s = (const void *)*src;
    size_t wn0 = wn;
    unsigned c = 0;

    if (st && (c = *(unsigned *)st)) {
        if (ws) {
            *(unsigned *)st = 0;
            goto resume;
        } else {
            goto resume0;
        }
    }

    if (MB_CUR_MAX == 1) {
        if (!ws) {
            return strlen((const char *)s);
        }

        for (;;) {
            if (!wn) {
                *src = (const void *)s;
                return wn0;
            }
            if (!*s)
                break;
            c = *s++;
            *ws++ = CODEUNIT(c);
            wn--;
        }
        *ws = 0;
        *src = 0;
        return wn0 - wn;
    }

    if (!ws) {
        for (;;) {
#ifdef __GNUC__
            typedef uint32_t __attribute__((__may_alias__)) w32;
            if (*s - 1u < 0x7F && (uintptr_t)s % 4 == 0) {
                while (
                  !(((*(w32 *)s) | (*(w32 *)s - 0x01010101)) & 0x80808080)) {
                    s += 4;
                    wn -= 4;
                }
            }
#endif
            if (*s - 1u < 0x7F) {
                s++;
                wn--;
                continue;
            }

            if (*s - SA > SB - SA) {
                break;
            }

            c = bittab[*s++ - SA];
        resume0:
            if (OOB(c, *s)) {
                s--;
                break;
            }

            s++;
            if (c & (1U << 25)) {
                if (*s - 0x80u >= 0x40) {
                    s -= 2;
                    break;
                }

                s++;
                if (c & (1U << 19)) {
                    if (*s - 0x80u >= 0x40) {
                        s -= 3;
                        break;
                    }
                    s++;
                }
            }
            wn--;
            c = 0;
        }
    } else {
        for (;;) {
            if (!wn) {
                *src = (const void *)s;
                return wn0;
            }
#ifdef __GNUC__
            typedef uint32_t __attribute__((__may_alias__)) w32;
            if (*s - 1u < 0x7F && (uintptr_t)s % 4 == 0) {
                while (wn >= 5 && !(((*(w32 *)s) | (*(w32 *)s - 0x01010101)) &
                                    0x80808080)) {
                    *ws++ = *s++;
                    *ws++ = *s++;
                    *ws++ = *s++;
                    *ws++ = *s++;
                    wn -= 4;
                }
            }
#endif
            if (*s - 1u < 0x7F) {
                *ws++ = *s++;
                wn--;
                continue;
            }

            if (*s - SA > SB - SA) {
                break;
            }

            c = bittab[*s++ - SA];
        resume:
            if (OOB(c, *s)) {
                s--;
                break;
            }

            c = (c << 6) | (*s++ - 0x80);
            if (c & (1U << 31)) {
                if (*s - 0x80u >= 0x40) {
                    s -= 2;
                    break;
                }

                c = (c << 6) | (*s++ - 0x80);
                if (c & (1U << 31)) {
                    if (*s - 0x80u >= 0x40) {
                        s -= 3;
                        break;
                    }
                    c = (c << 6) | (*s++ - 0x80);
                }
            }

            *ws++ = c;
            wn--;
            c = 0;
        }
    }

    if (!c && !*s) {
        if (ws) {
            *ws = 0;
            *src = 0;
        }

        return wn0 - wn;
    }

    errno = EILSEQ;
    if (ws) {
        *src = (const void *)s;
    }

    return -1;
}

size_t mbstowcs(wchar_t *restrict ws, const char *restrict s, size_t wn) {
    return mbsrtowcs(ws, (void *)&s, wn, 0);
}

size_t wcrtomb(char *restrict s, wchar_t wc) {
    if (!s) {
        return 1;
    }

    if ((unsigned)wc < 0x80) {
        *s = wc;
        return 1;
    } else if (MB_CUR_MAX == 1) {
        if (!IS_CODEUNIT(wc)) {
            errno = EILSEQ;
            return -1;
        }

        *s = wc;
        return 1;
    } else if ((unsigned)wc < 0x800) {
        *s++ = 0xC0 | (wc >> 6);
        *s = 0x80 | (wc & 0x3F);
        return 2;
    } else if ((unsigned)wc < 0xD800 || (unsigned)wc - 0xE000 < 0x2000) {
        *s++ = 0xE0 | (wc >> 12);
        *s++ = 0x80 | ((wc >> 6) & 0x3F);
        *s = 0x80 | (wc & 0x3F);
        return 3;
    } else if ((unsigned)wc - 0x10000 < 0x100000) {
        *s++ = 0xF0 | (wc >> 18);
        *s++ = 0x80 | ((wc >> 12) & 0x3F);
        *s++ = 0x80 | ((wc >> 6) & 0x3F);
        *s = 0x80 | (wc & 0x3F);
        return 4;
    }

    errno = EILSEQ;
    return -1;
}

int wctomb(char *s, wchar_t wc) {
    if (!s) {
        return 0;
    }

    return wcrtomb(s, wc);
}

size_t wcsrtombs(char *restrict s, const wchar_t **restrict ws, size_t n) {
    const wchar_t *ws2;
    char buf[4];
    size_t N = n, l;
    if (!s) {
        for (n = 0, ws2 = *ws; *ws2; ws2++) {
            if (*ws2 >= 0x80) {
                l = wcrtomb(buf, *ws2);
                if (!(l + 1)) {
                    return -1;
                }

                n += l;
            } else {
                n++;
            }
        }

        return n;
    }

    while (n >= 4) {
        if (**ws - 1u >= 0x7Fu) {
            if (!**ws) {
                *s = 0;
                *ws = 0;
                return N - n;
            }

            l = wcrtomb(s, **ws);
            if (!(l + 1)) {
                return -1;
            }

            s += l;
            n -= l;
        } else {
            *s++ = **ws;
            n--;
        }

        (*ws)++;
    }

    while (n) {
        if (**ws - 1u >= 0x7Fu) {
            if (!**ws) {
                *s = 0;
                *ws = 0;
                return N - n;
            }

            l = wcrtomb(buf, **ws);
            if (!(l + 1)) {
                return -1;
            }

            if (l > n) {
                return N - n;
            }

            wcrtomb(s, **ws);
            s += l;
            n -= l;
        } else {
            *s++ = **ws;
            n--;
        }

        (*ws)++;
    }

    return N;
}

size_t wcstombs(char *restrict s, const wchar_t *restrict ws, size_t n) {
    return wcsrtombs(s, &(const wchar_t *){ws}, n);
}

int globstatus;

_Noreturn void _Exit(int status) {
    putchar('\n');
    globstatus = status;
    extern int dis_print;
    dis_print = 1;
    shell("");
    for (;;)
        ;
}

_Noreturn void exit(int status) {
    while (regedhands != 0) {
        --regedhands;
        void (*function)(void) = atexitfunc[regedhands];
        function();
        putchar('\n');
    }

    for (;;) {
        _Exit(status);
    }
}

_Noreturn void abort() {
    kprint("Aborted");

    for (;;) {
        _Exit(EXIT_FAILURE);
    }
}

int atexit(void (*function)(void)) {
    atexitfunc[regedhands] = function;
    regedhands++;
    return 0;
}

_Noreturn void quick_exit(int status) {
    while (regedqhands != 0) {
        --regedqhands;
        void (*function)(void) = atqexitfunc[regedqhands];
        function();
        putchar('\n');
    }

    for (;;) {
        _Exit(status);
    }
}

int at_quick_exit(void (*function)(void)) {
    atqexitfunc[regedqhands] = function;
    regedqhands++;
    return 0;
}

char *getenv(const char *name) {
    // Stub, our OS doesn't supports environment variables yet
    (void)name;
    return NULL;
}
