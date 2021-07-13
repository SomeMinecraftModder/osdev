#include "math.h"
#include <ctype.h>
#include <errno.h>
#include <limits.h>
#include <signal.h>
#include <stdlib.h>
#define _GNU_SOURCE
#include <string.h>
#include <strings.h>

#define ALIGN      (sizeof(size_t))
#define ONES       ((size_t)-1 / UCHAR_MAX)
#define HIGHS      (ONES * (UCHAR_MAX / 2 + 1))
#define HASZERO(x) (((x)-ONES) & ~(x)&HIGHS)
#define BITOP(a, b, op)                                \
    ((a)[(size_t)(b) / (8 * sizeof *(a))] op(size_t) 1 \
     << ((size_t)(b) % (8 * sizeof *(a))))

int __errno = 0;

static const struct errmsgstr_t {
#define E(n, s) char str##n[sizeof(s)];
#include "strerror.h"
#undef E
} errmsgstr = {
#define E(n, s) s,
#include "strerror.h"
#undef E
};

static const uint16_t errmsgidx[] = {
#define E(n, s) [n] = offsetof(struct errmsgstr_t, str##n),
#include "strerror.h"
#undef E
};

char *int_to_ascii(int n, char str[]) {
    int i, sign;
    if ((sign = n) < 0) {
        n = -n;
    }

    i = 0;
    do {
        str[i++] = n % 10 + '0';
    } while ((n /= 10) > 0);

    if (sign < 0) {
        str[i++] = '-';
    }

    str[i] = '\0';

    reverse(str);

    return (str);
}

char *hex_to_ascii(int n, char str[]) {
    char zeros = 0;

    int i;
    for (i = 28; i >= 0; i -= 4) {
        int32_t tmp;
        tmp = (n >> i) & 0xF;
        if (tmp == 0 && zeros == 0) {
            continue;
        }
        zeros = 1;
        if (tmp >= 0xA) {
            append(str, tmp - 0xA + 'a');
        } else {
            append(str, tmp + '0');
        }
    }

    return (str);
}

char *hex_to_ascii_upper(int n, char str[]) {
    char zeros = 0;

    int32_t tmp;
    int i;
    for (i = 28; i >= 0; i -= 4) {
        tmp = (n >> i) & 0xF;
        if (tmp == 0 && zeros == 0) {
            continue;
        }
        zeros = 1;
        if (tmp >= 0xA) {
            append(str, tmp - 0xA + 'A');
        } else {
            append(str, tmp + '0');
        }
    }

    return (str);
}

char *alt_hex_to_ascii(int n, char str[]) {
    append(str, '0');
    append(str, 'x');
    char zeros = 0;

    int i;
    for (i = 28; i >= 0; i -= 4) {
        int32_t tmp;
        tmp = (n >> i) & 0xF;
        if (tmp == 0 && zeros == 0) {
            continue;
        }
        zeros = 1;
        if (tmp >= 0xA) {
            append(str, tmp - 0xA + 'a');
        } else {
            append(str, tmp + '0');
        }
    }

    return (str);
}

char *alt_hex_to_ascii_upper(int n, char str[]) {
    append(str, '0');
    append(str, 'x');
    char zeros = 0;

    int32_t tmp;
    int i;
    for (i = 28; i >= 0; i -= 4) {
        tmp = (n >> i) & 0xF;
        if (tmp == 0 && zeros == 0) {
            continue;
        }
        zeros = 1;
        if (tmp >= 0xA) {
            append(str, tmp - 0xA + 'A');
        } else {
            append(str, tmp + '0');
        }
    }

    return (str);
}

char *octal_to_ascii(int n) {
    static char representation[] = "01234567";
    static char buffer[50];
    char *str;

    str = &buffer[49];
    *str = '\0';

    do {
        *--str = representation[n % 8];
        n /= 8;
    } while (n != 0);

    return (str);
}

void *memcpy(void *restrict dest, const void *restrict src, size_t n) {
    uint8_t *d = dest;
    const uint8_t *s = src;

#ifdef __GNUC__
    typedef uint32_t __attribute__((__may_alias__)) u32;
    uint32_t w, x;

    for (; (uintptr_t)s % 4 && n; n--) {
        *d++ = *s++;
    }

    if ((uintptr_t)d % 4 == 0) {
        for (; n >= 16; s += 16, d += 16, n -= 16) {
            *(u32 *)(d + 0) = *(u32 *)(s + 0);
            *(u32 *)(d + 4) = *(u32 *)(s + 4);
            *(u32 *)(d + 8) = *(u32 *)(s + 8);
            *(u32 *)(d + 12) = *(u32 *)(s + 12);
        }

        if (n & 8) {
            *(u32 *)(d + 0) = *(u32 *)(s + 0);
            *(u32 *)(d + 4) = *(u32 *)(s + 4);
            d += 8;
            s += 8;
        }

        if (n & 4) {
            *(u32 *)(d + 0) = *(u32 *)(s + 0);
            d += 4;
            s += 4;
        }

        if (n & 2) {
            *d++ = *s++;
            *d++ = *s++;
        }

        if (n & 1) {
            *d = *s;
        }

        return dest;
    }

    if (n >= 32)
        switch ((uintptr_t)d % 4) {
            case 1:
                w = *(u32 *)s;
                *d++ = *s++;
                *d++ = *s++;
                *d++ = *s++;
                n -= 3;
                for (; n >= 17; s += 16, d += 16, n -= 16) {
                    x = *(u32 *)(s + 1);
                    *(u32 *)(d + 0) = (w >> 24) | (x << 8);
                    w = *(u32 *)(s + 5);
                    *(u32 *)(d + 4) = (x >> 24) | (w << 8);
                    x = *(u32 *)(s + 9);
                    *(u32 *)(d + 8) = (w >> 24) | (x << 8);
                    w = *(u32 *)(s + 13);
                    *(u32 *)(d + 12) = (x >> 24) | (w << 8);
                }
                break;

            case 2:
                w = *(u32 *)s;
                *d++ = *s++;
                *d++ = *s++;
                n -= 2;
                for (; n >= 18; s += 16, d += 16, n -= 16) {
                    x = *(u32 *)(s + 2);
                    *(u32 *)(d + 0) = (w >> 16) | (x << 16);
                    w = *(u32 *)(s + 6);
                    *(u32 *)(d + 4) = (x >> 16) | (w << 16);
                    x = *(u32 *)(s + 10);
                    *(u32 *)(d + 8) = (w >> 16) | (x << 16);
                    w = *(u32 *)(s + 14);
                    *(u32 *)(d + 12) = (x >> 16) | (w << 16);
                }
                break;

            case 3:
                w = *(u32 *)s;
                *d++ = *s++;
                n -= 1;
                for (; n >= 19; s += 16, d += 16, n -= 16) {
                    x = *(u32 *)(s + 3);
                    *(u32 *)(d + 0) = (w >> 8) | (x << 24);
                    w = *(u32 *)(s + 7);
                    *(u32 *)(d + 4) = (x >> 8) | (w << 24);
                    x = *(u32 *)(s + 11);
                    *(u32 *)(d + 8) = (w >> 8) | (x << 24);
                    w = *(u32 *)(s + 15);
                    *(u32 *)(d + 12) = (x >> 8) | (w << 24);
                }
                break;
        }

    if (n & 16) {
        *d++ = *s++;
        *d++ = *s++;
        *d++ = *s++;
        *d++ = *s++;
        *d++ = *s++;
        *d++ = *s++;
        *d++ = *s++;
        *d++ = *s++;
        *d++ = *s++;
        *d++ = *s++;
        *d++ = *s++;
        *d++ = *s++;
        *d++ = *s++;
        *d++ = *s++;
        *d++ = *s++;
        *d++ = *s++;
    }

    if (n & 8) {
        *d++ = *s++;
        *d++ = *s++;
        *d++ = *s++;
        *d++ = *s++;
        *d++ = *s++;
        *d++ = *s++;
        *d++ = *s++;
        *d++ = *s++;
    }

    if (n & 4) {
        *d++ = *s++;
        *d++ = *s++;
        *d++ = *s++;
        *d++ = *s++;
    }

    if (n & 2) {
        *d++ = *s++;
        *d++ = *s++;
    }

    if (n & 1) {
        *d = *s;
    }

    return dest;
#endif

    for (; n; n--) {
        *d++ = *s++;
    }
    return dest;
}

void *memccpy(void *restrict dest, const void *restrict src, int c, size_t n) {
    uint8_t *d = dest;
    const uint8_t *s = src;

    c = (uint8_t)c;
#ifdef __GNUC__
    typedef size_t __attribute__((__may_alias__)) word;
    word *wd;
    const word *ws;
    if (((uintptr_t)s & (ALIGN - 1)) == ((uintptr_t)d & (ALIGN - 1))) {
        for (; ((uintptr_t)s & (ALIGN - 1)) && n && (*d = *s) != c;
             n--, s++, d++)
            ;
        if ((uintptr_t)s & (ALIGN - 1)) {
            goto tail;
        }

        size_t k = ONES * c;
        wd = (void *)d;
        ws = (const void *)s;
        for (; n >= sizeof(size_t) && !HASZERO(*ws ^ k);
             n -= sizeof(size_t), ws++, wd++) {
            *wd = *ws;
        }

        d = (void *)wd;
        s = (const void *)ws;
    }
#endif
    for (; n && (*d = *s) != c; n--, s++, d++)
        ;
tail:
    if (n) {
        return d + 1;
    }
    return 0;
}

void *mempcpy(void *dest, const void *src, size_t n) {
    return (char *)memcpy(dest, src, n) + n;
}

void *memset(void *dest, uint8_t c, size_t n) {
    uint8_t *s = dest;
    size_t k;

    if (!n) {
        return dest;
    }

    s[0] = c;
    s[n - 1] = c;
    if (n <= 2) {
        return dest;
    }

    s[1] = c;
    s[2] = c;
    s[n - 2] = c;
    s[n - 3] = c;
    if (n <= 6) {
        return dest;
    }

    s[3] = c;
    s[n - 4] = c;
    if (n <= 8) {
        return dest;
    }

    k = -(uintptr_t)s & 3;
    s += k;
    n -= k;
    n &= -4;

#ifdef __GNUC__
    typedef uint32_t __attribute__((__may_alias__)) u32;
    typedef uint64_t __attribute__((__may_alias__)) u64;

    u32 c32 = ((u32)-1) / 255 * (unsigned char)c;

    *(u32 *)(s + 0) = c32;
    *(u32 *)(s + n - 4) = c32;
    if (n <= 8) {
        return dest;
    }

    *(u32 *)(s + 4) = c32;
    *(u32 *)(s + 8) = c32;
    *(u32 *)(s + n - 12) = c32;
    *(u32 *)(s + n - 8) = c32;
    if (n <= 24) {
        return dest;
    }

    *(u32 *)(s + 12) = c32;
    *(u32 *)(s + 16) = c32;
    *(u32 *)(s + 20) = c32;
    *(u32 *)(s + 24) = c32;
    *(u32 *)(s + n - 28) = c32;
    *(u32 *)(s + n - 24) = c32;
    *(u32 *)(s + n - 20) = c32;
    *(u32 *)(s + n - 16) = c32;

    k = 24 + ((uintptr_t)s & 4);
    s += k;
    n -= k;

    u64 c64 = c32 | ((u64)c32 << 32);
    for (; n >= 32; n -= 32, s += 32) {
        *(u64 *)(s + 0) = c64;
        *(u64 *)(s + 8) = c64;
        *(u64 *)(s + 16) = c64;
        *(u64 *)(s + 24) = c64;
    }
#else
    // Pure C fallback with no aliasing violations
    for (; n; n--, s++) {
        *s = c;
    }
#endif

    return dest;
}

#ifdef __GNUC__
typedef __attribute__((__may_alias__)) size_t WT;
    #define WS (sizeof(WT))
#endif

void *memmove(void *dest, const void *src, size_t n) {
    char *d = dest;
    const char *s = src;

    if (d == s) {
        return d;
    }

    if ((uintptr_t)s - (uintptr_t)d - n <= -2 * n) {
        return memcpy(d, s, n);
    }

    if (d < s) {
#ifdef __GNUC__
        if ((uintptr_t)s % WS == (uintptr_t)d % WS) {
            while ((uintptr_t)d % WS) {
                if (!n--) {
                    return dest;
                }

                *d++ = *s++;
            }

            for (; n >= WS; n -= WS, d += WS, s += WS) {
                *(WT *)d = *(WT *)s;
            }
        }
#endif
        for (; n; n--) {
            *d++ = *s++;
        }
    } else {
#ifdef __GNUC__
        if ((uintptr_t)s % WS == (uintptr_t)d % WS) {
            while ((uintptr_t)(d + n) % WS) {
                if (!n--) {
                    return dest;
                }

                d[n] = s[n];
            }

            while (n >= WS) {
                n -= WS, *(WT *)(d + n) = *(WT *)(s + n);
            }
        }
#endif
        while (n) {
            n--, d[n] = s[n];
        }
    }

    return dest;
}

void *memchr(const void *src, int c, size_t n) {
    const uint8_t *s = src;
    c = (uint8_t)c;
#ifdef __GNUC__
    for (; ((uintptr_t)s & (ALIGN - 1)) && n && *s != c; s++, n--)
        ;
    if (n && *s != c) {
        typedef size_t __attribute__((__may_alias__)) word;
        const word *w;
        size_t k = ONES * c;
        for (w = (const void *)s; n >= ALIGN && !HASZERO(*w ^ k);
             w++, n -= ALIGN)
            ;
        s = (const void *)w;
    }
#endif
    for (; n && *s != c; s++, n--)
        ;
    return n ? (void *)s : 0;
}

void *memrchr(const void *m, int c, size_t n) {
    const uint8_t *s = m;
    c = (uint8_t)c;
    while (n--) {
        if (s[n] == c) {
            return (void *)(s + n);
        }
    }

    return 0;
}

int memcmp(const void *vl, const void *vr, size_t n) {
    const uint8_t *l = vl, *r = vr;
    for (; n && *l == *r; n--, l++, r++)
        ;
    return n ? *l - *r : 0;
}

int startswith(char *str, char *accept) {
    size_t s = strlen(accept);

    for (size_t i = 0; i < s; ++i) {
        if (*str != *accept) {
            return 0;
        }
        str++;
        accept++;
    }

    return 1;
}

char *reverse(char s[]) {
    int i, j;

    for (i = 0, j = strlen(s) - 1; i < j; i++, j--) {
        int c;
        c = s[i];
        s[i] = s[j];
        s[j] = c;
    }

    return s;
}

void swap(int a, int b) {
    int t = a;
    a = b;
    b = t;
}

void append(char s[], char n) {
    int len = strlen(s);
    s[len] = n;
    s[len + 1] = '\0';
}

void backspace(char s[]) {
    int len = strlen(s);
    s[len - 1] = '\0';
}

int strcmp(const char s1[], const char s2[]) {
    int i;

    for (i = 0; s1[i] == s2[i]; i++) {
        if (s1[i] == '\0')
            return 0;
    }

    return s1[i] - s2[i];
}

int strncmp(const char s1[], const char s2[], size_t n) {
    int i;

    for (i = 0; n && s1[i] == s2[i]; ++i, --n) {
        if (s1[i] == '\0') {
            return 0;
        }
    }

    return (s1[i] - s2[i]);
}

size_t strlen(const char *s) {
    const char *a = s;
#ifdef __GNUC__
    typedef size_t __attribute__((__may_alias__)) word;
    const word *w;
    for (; (uintptr_t)s % ALIGN; s++) {
        if (!*s) {
            return s - a;
        }
    }

    for (w = (const void *)s; !HASZERO(*w); w++)
        ;
    s = (const void *)w;
#endif
    for (; *s; s++)
        ;
    return s - a;
}

size_t strnlen(const char *s, int32_t n) {
    const char *p = memchr(s, 0, n);
    return p ? p - s : n;
}

char *strcpy(char *restrict dest, const char *restrict src) {
    stpcpy(dest, src);
    return dest;
}

size_t strlcpy(char *d, const char *s, size_t n) {
    char *d0 = d;

    if (!n--) {
        goto finish;
    }

    for (; n && (*d = *s); n--, s++, d++)
        ;
    *d = 0;
finish:
    return d - d0 + strlen(s);
}

size_t strlcat(char *d, const char *s, size_t n) {
    size_t l = strnlen(d, n);
    if (l == n) {
        return l + strlen(s);
    }

    return l + strlcpy(d + l, s, n - l);
}

char *strncpy(char *restrict dest, const char *restrict src, size_t n) {
    stpncpy(dest, src, n);
    return dest;
}

char *stpcpy(char *restrict d, const char *restrict s) {
#ifdef __GNUC__
    typedef size_t __attribute__((__may_alias__)) word;
    word *wd;
    const word *ws;
    if ((uintptr_t)s % ALIGN == (uintptr_t)d % ALIGN) {
        for (; (uintptr_t)s % ALIGN; s++, d++)
            if (!(*d = *s))
                return d;
        wd = (void *)d;
        ws = (const void *)s;
        for (; !HASZERO(*ws); *wd++ = *ws++)
            ;
        d = (void *)wd;
        s = (const void *)ws;
    }
#endif
    for (; (*d = *s); s++, d++)
        ;

    return d;
}

char *stpncpy(char *restrict d, const char *restrict s, size_t n) {
#ifdef __GNUC__
    typedef size_t __attribute__((__may_alias__)) word;
    word *wd;
    const word *ws;
    if (((uintptr_t)s & (ALIGN - 1)) == ((uintptr_t)d & (ALIGN - 1))) {
        for (; ((uintptr_t)s & (ALIGN - 1)) && n && (*d = *s); n--, s++, d++)
            ;
        if (!n || !*s) {
            goto tail;
        }

        wd = (void *)d;
        ws = (const void *)s;
        for (; n >= sizeof(size_t) && !HASZERO(*ws);
             n -= sizeof(size_t), ws++, wd++) {
            *wd = *ws;
        }

        d = (void *)wd;
        s = (const void *)ws;
    }
#endif
    for (; n && (*d = *s); n--, s++, d++)
        ;
tail:
    memset(d, 0, n);
    return d;
}

char *strcat(char *restrict dest, const char *restrict src) {
    strcpy(dest + strlen(dest), src);
    return dest;
}

char *strncat(char *restrict dest, const char *restrict src, size_t n) {
    char *ptr = dest + strlen(dest);

    while (n && *src) {
        n--;
        *ptr++ = *src++;
    }

    *ptr++ = '\0';
    return dest;
}

static char *twobyte_strstr(const uint8_t *h, const uint8_t *n) {
    uint16_t nw = n[0] << 8 | n[1], hw = h[0] << 8 | h[1];
    for (h++; *h && hw != nw; hw = hw << 8 | *++h)
        ;
    return *h ? (char *)h - 1 : 0;
}

static char *threebyte_strstr(const uint8_t *h, const uint8_t *n) {
    uint32_t nw = (uint32_t)n[0] << 24 | n[1] << 16 | n[2] << 8;
    uint32_t hw = (uint32_t)h[0] << 24 | h[1] << 16 | h[2] << 8;
    for (h += 2; *h && hw != nw; hw = (hw | *++h) << 8)
        ;
    return *h ? (char *)h - 2 : 0;
}

static char *fourbyte_strstr(const uint8_t *h, const uint8_t *n) {
    uint32_t nw = (uint32_t)n[0] << 24 | n[1] << 16 | n[2] << 8 | n[3];
    uint32_t hw = (uint32_t)h[0] << 24 | h[1] << 16 | h[2] << 8 | h[3];
    for (h += 3; *h && hw != nw; hw = hw << 8 | *++h)
        ;
    return *h ? (char *)h - 3 : 0;
}

static char *twoway_strstr(const uint8_t *h, const uint8_t *n) {
    const uint8_t *z;
    size_t l, ip, jp, k, p, ms, p0, mem, mem0;
    size_t byteset[32 / sizeof(size_t)] = {0};
    size_t shift[256];

    // Computing length of needle and fill shift table
    for (l = 0; n[l] && h[l]; l++) {
        BITOP(byteset, n[l], |=), shift[n[l]] = l + 1;
    }

    if (n[l]) {
        return 0; // Hit the end of h
    }

    // Compute maximal suffix
    ip = -1;
    jp = 0;
    k = p = 1;
    while (jp + k < l) {
        if (n[ip + k] == n[jp + k]) {
            if (k == p) {
                jp += p;
                k = 1;
            } else
                k++;
        } else if (n[ip + k] > n[jp + k]) {
            jp += k;
            k = 1;
            p = jp - ip;
        } else {
            ip = jp++;
            k = p = 1;
        }
    }
    ms = ip;
    p0 = p;

    // And with the opposite comparison
    ip = -1;
    jp = 0;
    k = p = 1;
    while (jp + k < l) {
        if (n[ip + k] == n[jp + k]) {
            if (k == p) {
                jp += p;
                k = 1;
            } else
                k++;
        } else if (n[ip + k] < n[jp + k]) {
            jp += k;
            k = 1;
            p = jp - ip;
        } else {
            ip = jp++;
            k = p = 1;
        }
    }

    if (ip + 1 > ms + 1) {
        ms = ip;
    } else {
        p = p0;
    }

    // Periodic needle?
    if (memcmp(n, n + p, ms + 1)) {
        mem0 = 0;
        p = max(ms, l - ms - 1) + 1;
    } else {
        mem0 = l - p;
    }
    mem = 0;

    // Initialize incremental end-of-haystack pointer
    z = h;

    // Search loop
    for (;;) {
        // Update incremental end-of-haystack pointer
        if (z - h < (int32_t)l) {
            // Fast estimate for max(l, 63)
            size_t grow = l | 63;
            const uint8_t *z2 = memchr(z, 0, grow);
            if (z2) {
                z = z2;
                if (z - h < (int32_t)l)
                    return 0;
            } else {
                z += grow;
            }
        }

        // Check last byte first; advance by shift on mismatch
        if (BITOP(byteset, h[l - 1], &)) {
            k = l - shift[h[l - 1]];
            if (k) {
                if (k < mem)
                    k = mem;
                h += k;
                mem = 0;
                continue;
            }
        } else {
            h += l;
            mem = 0;
            continue;
        }

        // Compare right half
        for (k = max(ms + 1, mem); n[k] && n[k] == h[k]; k++)
            ;
        if (n[k]) {
            h += k - ms;
            mem = 0;
            continue;
        }

        // Compare left half
        for (k = ms + 1; k > mem && n[k - 1] == h[k - 1]; k--)
            ;
        if (k <= mem) {
            return (char *)h;
        }
        h += p;
        mem = mem0;
    }
}

char *strstr(const char *h, const char *n) {
    // Return immediately on empty needle
    if (!n[0]) {
        return (char *)h;
    }

    // Use faster algorithms for short needles
    h = strchr(h, *n);
    if (!h || !n[1]) {
        return (char *)h;
    }

    if (!h[1]) {
        return 0;
    }

    if (!n[2]) {
        return twobyte_strstr((void *)h, (void *)n);
    }

    if (!h[2]) {
        return 0;
    }

    if (!n[3]) {
        return threebyte_strstr((void *)h, (void *)n);
    }

    if (!h[3]) {
        return 0;
    }

    if (!n[4]) {
        return fourbyte_strstr((void *)h, (void *)n);
    }

    return twoway_strstr((void *)h, (void *)n);
}

char *strrstr(const char *h, const char *n) {
    char *r = NULL;

    if (!n[0]) {
        return (char *)h + strlen(h);
    }

    while (1) {
        char *p = strstr(h, n);
        if (!p) {
            return r;
        }
        r = p;
        h = p + 1;
    }
}

char *strchrnul(const char *s, int c) {
    c = (uint8_t)c;
    if (!c) {
        return (char *)s + strlen(s);
    }

#ifdef __GNUC__
    typedef size_t __attribute__((__may_alias__)) word;
    const word *w;
    for (; (uintptr_t)s % ALIGN; s++) {
        if (!*s || *(uint8_t *)s == c) {
            return (char *)s;
        }
    }
    size_t k = ONES * c;
    for (w = (void *)s; !HASZERO(*w) && !HASZERO(*w ^ k); w++)
        ;
    s = (void *)w;
#endif
    for (; *s && *(uint8_t *)s != c; s++)
        ;
    return (char *)s;
}

char *strchr(const char *s, int c) {
    char *r = strchrnul(s, c);
    return *(uint8_t *)r == (uint8_t)c ? r : 0;
}

char *strrchr(const char *s, int c) {
    return memrchr(s, c, strlen(s) + 1);
}

size_t strspn(const char *s, const char *c) {
    const char *a = s;
    size_t byteset[32 / sizeof(size_t)] = {0};

    if (!c[0]) {
        return 0;
    }

    if (!c[1]) {
        for (; *s == *c; s++)
            ;
        return s - a;
    }

    for (; *c && BITOP(byteset, *(uint8_t *)c, |=); c++)
        ;
    for (; *s && BITOP(byteset, *(uint8_t *)s, &); s++)
        ;
    return s - a;
}

size_t strcspn(const char *s, const char *c) {
    const char *a = s;
    size_t byteset[32 / sizeof(size_t)];

    if (!c[0] || !c[1]) {
        return strchrnul(s, *c) - a;
    }

    memset(byteset, 0, sizeof byteset);
    for (; *c && BITOP(byteset, *(uint8_t *)c, |=); c++)
        ;
    for (; *s && !BITOP(byteset, *(uint8_t *)s, &); s++)
        ;
    return s - a;
}

char *strtok(char *restrict s, const char *restrict sep) {
    static char *p;
    if (!s && !(s = p)) {
        return NULL;
    }

    s += strspn(s, sep);
    if (!*s) {
        p = 0;
    }

    p = s + strcspn(s, sep);
    if (*p) {
        *p++ = 0;
    } else {
        p = 0;
    }

    return s;
}

char *strtok_r(char *restrict s, const char *restrict sep, char **restrict p) {
    if (!s && !(s = *p)) {
        return NULL;
    }

    s += strspn(s, sep);
    if (!*s) {
        return *p = 0;
    }

    *p = s + strcspn(s, sep);
    if (**p) {
        *(*p)++ = 0;
    } else {
        *p = 0;
    }

    return s;
}

char *strpbrk(const char *s, const char *b) {
    s += strcspn(s, b);
    return *s ? (char *)s : 0;
}

int strcoll(const char *l, const char *r) {
    return strcmp(l, r);
}

size_t strxfrm(char *restrict dest, const char *restrict src, size_t n) {
    size_t l = strlen(src);
    if (n > l) {
        strcpy(dest, src);
    }

    return l;
}

char *strdup(const char *s) {
    size_t l = strlen(s);
    char *d = (char *)malloc(l + 1);
    if (!d) {
        return NULL;
    }

    return memcpy(d, s, l + 1);
}

char *strndup(const char *s, size_t n) {
    size_t l = strnlen(s, n);
    char *d = (char *)malloc(l + 1);
    if (!d) {
        return NULL;
    }

    memcpy(d, s, l);
    d[l] = 0;
    return d;
}

char *strsep(char **str, const char *sep) {
    char *s = *str, *end;

    if (!s) {
        return NULL;
    }

    end = s + strcspn(s, sep);
    if (*end) {
        *end++ = 0;
    } else {
        end = 0;
    }

    *str = end;
    return s;
}

char *strerror(int e) {
    const char *s;
    if ((uint32_t)e >= sizeof(errmsgidx) / sizeof(*errmsgidx)) {
        e = 0;
    }

    s = (char *)&errmsgstr + errmsgidx[e];
    return (char *)s;
}

int strerror_r(int err, char *buf, size_t buflen) {
    char *msg = strerror(err);
    size_t l = strlen(msg);
    if (l >= buflen) {
        if (buflen) {
            memcpy(buf, msg, buflen - 1);
            buf[buflen - 1] = 0;
        }
        return ERANGE;
    }

    memcpy(buf, msg, l + 1);
    return 0;
}

#if (SIGHUP == 1) && (SIGINT == 2) && (SIGQUIT == 3) && (SIGILL == 4) &&       \
  (SIGTRAP == 5) && (SIGABRT == 6) && (SIGBUS == 7) && (SIGFPE == 8) &&        \
  (SIGKILL == 9) && (SIGUSR1 == 10) && (SIGSEGV == 11) && (SIGUSR2 == 12) &&   \
  (SIGPIPE == 13) && (SIGALRM == 14) && (SIGTERM == 15) &&                     \
  (SIGSTKFLT == 16) && (SIGCHLD == 17) && (SIGCONT == 18) &&                   \
  (SIGSTOP == 19) && (SIGTSTP == 20) && (SIGTTIN == 21) && (SIGTTOU == 22) &&  \
  (SIGURG == 23) && (SIGXCPU == 24) && (SIGXFSZ == 25) && (SIGVTALRM == 26) && \
  (SIGPROF == 27) && (SIGWINCH == 28) && (SIGPOLL == 29) && (SIGPWR == 30) &&  \
  (SIGSYS == 31)

    #define sigmap(x) x

#else

static const char map[] = {
  [SIGHUP] = 1,     [SIGINT] = 2,     [SIGQUIT] = 3,  [SIGILL] = 4,
  [SIGTRAP] = 5,    [SIGABRT] = 6,    [SIGBUS] = 7,   [SIGFPE] = 8,
  [SIGKILL] = 9,    [SIGUSR1] = 10,   [SIGSEGV] = 11, [SIGUSR2] = 12,
  [SIGPIPE] = 13,   [SIGALRM] = 14,   [SIGTERM] = 15,
    #if defined(SIGSTKFLT)
  [SIGSTKFLT] = 16,
    #elif defined(SIGEMT)
  [SIGEMT] = 16,
    #endif
  [SIGCHLD] = 17,   [SIGCONT] = 18,   [SIGSTOP] = 19, [SIGTSTP] = 20,
  [SIGTTIN] = 21,   [SIGTTOU] = 22,   [SIGURG] = 23,  [SIGXCPU] = 24,
  [SIGXFSZ] = 25,   [SIGVTALRM] = 26, [SIGPROF] = 27, [SIGWINCH] = 28,
  [SIGPOLL] = 29,   [SIGPWR] = 30,    [SIGSYS] = 31};

    #define sigmap(x) ((x) >= sizeof map ? (x) : map[(x)])

#endif

static const char strings[] =
  "Unknown signal\0"
  "Hangup\0"
  "Interrupt\0"
  "Quit\0"
  "Illegal instruction\0"
  "Trace/breakpoint trap\0"
  "Aborted\0"
  "Bus error\0"
  "Arithmetic exception\0"
  "Killed\0"
  "User defined signal 1\0"
  "Segmentation fault\0"
  "User defined signal 2\0"
  "Broken pipe\0"
  "Alarm clock\0"
  "Terminated\0"
#if defined(SIGSTKFLT)
  "Stack fault\0"
#elif defined(SIGEMT)
  "Emulator trap\0"
#else
  "Unknown signal\0"
#endif
  "Child process status\0"
  "Continued\0"
  "Stopped (signal)\0"
  "Stopped\0"
  "Stopped (tty input)\0"
  "Stopped (tty output)\0"
  "Urgent I/O condition\0"
  "CPU time limit exceeded\0"
  "File size limit exceeded\0"
  "Virtual timer expired\0"
  "Profiling timer expired\0"
  "Window changed\0"
  "I/O possible\0"
  "Power failure\0"
  "Bad system call\0"
  "RT32"
  "\0RT33\0RT34\0RT35\0RT36\0RT37\0RT38\0RT39\0RT40"
  "\0RT41\0RT42\0RT43\0RT44\0RT45\0RT46\0RT47\0RT48"
  "\0RT49\0RT50\0RT51\0RT52\0RT53\0RT54\0RT55\0RT56"
  "\0RT57\0RT58\0RT59\0RT60\0RT61\0RT62\0RT63\0RT64"
#if _NSIG > 65
  "\0RT65\0RT66\0RT67\0RT68\0RT69\0RT70\0RT71\0RT72"
  "\0RT73\0RT74\0RT75\0RT76\0RT77\0RT78\0RT79\0RT80"
  "\0RT81\0RT82\0RT83\0RT84\0RT85\0RT86\0RT87\0RT88"
  "\0RT89\0RT90\0RT91\0RT92\0RT93\0RT94\0RT95\0RT96"
  "\0RT97\0RT98\0RT99\0RT100\0RT101\0RT102\0RT103\0RT104"
  "\0RT105\0RT106\0RT107\0RT108\0RT109\0RT110\0RT111\0RT112"
  "\0RT113\0RT114\0RT115\0RT116\0RT117\0RT118\0RT119\0RT120"
  "\0RT121\0RT122\0RT123\0RT124\0RT125\0RT126\0RT127\0RT128"
#endif
  "";

char *strsignal(int signum) {
    const char *s = strings;

    signum = sigmap(signum);
    if (signum - 1U >= _NSIG - 1) {
        signum = 0;
    }

    for (; signum--; s++) {
        for (; *s; s++)
            ;
    }

    return (char *)s;
}

int strverscmp(const char *l0, const char *r0) {
    const uint8_t *l = (const void *)l0;
    const uint8_t *r = (const void *)r0;
    size_t i, dp, j;
    int z = 1;

    for (dp = i = 0; l[i] == r[i]; i++) {
        int c = l[i];
        if (!c) {
            return 0;
        }

        if (!isdigit(c)) {
            dp = i + 1;
            z = 1;
        } else if (c != '0') {
            z = 0;
        }
    }

    if (l[dp] != '0' && r[dp] != '0') {
        for (j = i; isdigit(l[j]); j++) {
            if (!isdigit(r[j])) {
                return 1;
            }
        }

        if (isdigit(r[j])) {
            return -1;
        }
    } else if (z && dp < i && (isdigit(l[i]) || isdigit(r[i]))) {
        return (uint8_t)(l[i] - '0') - (uint8_t)(r[i] - '0');
    }

    return l[i] - r[i];
}

char *strcasestr(const char *h, const char *n) {
    size_t l = strlen(n);
    for (; *h; h++) {
        if (!strncasecmp(h, n, l)) {
            return (char *)h;
        }
    }

    return 0;
}

char *strtruncate(char *str, int n) {
    if (n <= 0) {
        return str;
    }

    int l = n;
    int len = strlen(str);
    if (n > len) {
        l = len;
    }

    str[len - l] = '\0';
    return str;
}

int *__errno_location() {
    return &__errno;
}
