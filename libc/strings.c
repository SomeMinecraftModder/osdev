#include <ctype.h>
#include <string.h>
#include <strings.h>

#ifdef __STRICT_ANSI__
    #define asm __asm__
#endif

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

int ffs(int i) {
    return i ? a_ctz_l(i) + 1 : 0;
}

int ffsl(long i) {
    return i ? a_ctz_l(i) + 1 : 0;
}

int ffsll(long long i) {
    return i ? a_ctz_64(i) + 1 : 0;
}

int bcmp(const void *s1, const void *s2, size_t n) {
    return memcmp(s1, s2, n);
}

void bcopy(const void *s1, void *s2, size_t n) {
    memmove(s2, s1, n);
}

void bzero(void *s, size_t n) {
    memset(s, 0, n);
}

char *index(const char *s, int c) {
    return strchr(s, c);
}

char *rindex(const char *s, int c) {
    return strrchr(s, c);
}

int strcasecmp(const char *_l, const char *_r) {
    const uint8_t *l = (void *)_l, *r = (void *)_r;
    for (; *l && *r && (*l == *r || tolower(*l) == tolower(*r)); l++, r++)
        ;
    return tolower(*l) - tolower(*r);
}

int strncasecmp(const char *_l, const char *_r, size_t n) {
    const uint8_t *l = (void *)_l, *r = (void *)_r;
    if (!n--) {
        return 0;
    }
    for (; *l && *r && n && (*l == *r || tolower(*l) == tolower(*r));
         l++, r++, n--)
        ;
    return tolower(*l) - tolower(*r);
}
