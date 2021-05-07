#include "string.h"
#include "ctype.h"
#include "kheap.h"
#include "math.h"

#define BITOP(a, b, op)                                \
    ((a)[(size_t)(b) / (8 * sizeof *(a))] op(size_t) 1 \
     << ((size_t)(b) % (8 * sizeof *(a))))

/**
 * K&R implementation
 */
char *int_to_ascii(int n, char str[]) {
    int i, sign;
    if ((sign = n) < 0)
        n = -n;
    i = 0;
    do {
        str[i++] = n % 10 + '0';
    } while ((n /= 10) > 0);

    if (sign < 0)
        str[i++] = '-';
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

void *memcpy(void *dest, const void *src, size_t nbytes) {
    uint8_t *q = (uint8_t *)dest;
    uint8_t *p = (uint8_t *)src;
    uint8_t *end = p + nbytes;

    while (p != end) {
        *q++ = *p++;
    }

    return dest;
}

void *memccpy(void *restrict dest, const void *restrict src, int c, size_t n) {
    uint8_t *d = dest;
    const uint8_t *s = src;

    c = (uint8_t)c;
    for (; n && (*d = *s) != c; n--, s++, d++)
        ;
    if (n) {
        return d + 1;
    }
    return 0;
}

void *mempcpy(void *dest, const void *src, size_t nbytes) {
    return (char *)memcpy(dest, src, nbytes) + nbytes;
}

void *memset(void *dest, uint8_t val, size_t len) {
    uint8_t *temp = (uint8_t *)dest;
    for (; len != 0; len--)
        *temp++ = val;
    return dest;
}

void *memmove(void *dest, const void *src, size_t nbytes) {
    uint8_t *p = (uint8_t *)src;
    uint8_t *q = (uint8_t *)dest;
    uint8_t *end = p + nbytes;

    if (q > p && q < end) {
        p = end;
        q += nbytes;

        while (p != src) {
            *--q = *--p;
        }
    } else {
        while (p != end) {
            *q++ = *p++;
        }
    }

    return dest;
}

void *memchr(const void *buf, int c, size_t n) {
    const uint8_t *p = (uint8_t *)buf;
    c = (uint8_t)c;

    for (; n && *p != c; p++, n--)
        ;
    return n ? (void *)p : 0;
}

void *memrchr(const void *buf, int c, size_t n) {
    const uint8_t *s = buf;
    c = (uint8_t)c;
    while (n--) {
        if (s[n] == c) {
            return (void *)(s + n);
        }
    }

    return 0;
}

int memcmp(const void *s1, const void *s2, size_t n) {
    uint8_t *byte1 = (uint8_t *)s1;
    uint8_t *byte2 = (uint8_t *)s2;
    while ((*byte1 == *byte2) && (n > 0)) {
        ++byte1;
        ++byte2;
        --n;
    }

    if (n == 0) {
        return 0;
    }
    return *byte1 - *byte2;
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

/* K&R
 * Returns <0 if s1<s2, 0 if s1==s2, >0 if s1>s2 */
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
    int i = 0;

    while (s[i] != '\0') {
        ++i;
    }

    return i;
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

char *strncpy(char *restrict dest, const char *restrict src, size_t n) {
    stpncpy(dest, src, n);
    return dest;
}

char *stpcpy(char *restrict d, const char *restrict s) {
    for (; (*d = *s); s++, d++)
        ;

    return d;
}

char *stpncpy(char *restrict d, const char *restrict s, size_t n) {
    for (; n && (*d = *s); n--, s++, d++)
        ;
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
        if (z - h < (long signed int)l) {
            // Fast estimate for max(l, 63)
            size_t grow = l | 63;
            const uint8_t *z2 = memchr(z, 0, grow);
            if (z2) {
                z = z2;
                if (z - h < (long signed int)l)
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
    char *d = (char *)kmalloc(l + 1);
    if (!d) {
        return NULL;
    }
    return memcpy(d, s, l + 1);
}

char *strndup(const char *s, size_t n) {
    size_t l = strnlen(s, n);
    char *d = (char *)kmalloc(l + 1);
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

char *strtruncate(char *str, int n) {
    if (n <= 0)
        return str;
    int l = n;
    int len = strlen(str);
    if (n > len)
        l = len;
    str[len - l] = '\0';
    return str;
}
