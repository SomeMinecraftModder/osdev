#include "../drivers/serial.h"
#include "mem.h"

extern char start;

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
    for (; n && (*d = *s) != c; n--, s++, d++);
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
    for (; len != 0; len--) *temp++ = val;
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

    for (; n && *p != c; p++, n--);
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

static uint32_t memchar(uint32_t val) {
    return (val >= 0x20 && val < 0x80) ? val : '.';
}

void memdump(const void *start, const void *end) {
    printf_serial("Memory dump from %X to %X\n", start, end);

    uint8_t *p = (uint8_t *)start;

    while (p < (uint8_t *)end) {
        uint32_t offset = (uint32_t)p & 0xFFFF;
        printf_serial("%X:  "
                "%X %X %X %X  "
                "%X %X %X %X  "
                "%X %X %X %X  "
                "%X %X %X %X  "
                "%c%c%c%c%c%c%c%c"
                "%c%c%c%c%c%c%c%c\n",
                offset,
                p[0x0], p[0x1], p[0x2], p[0x3],
                p[0x4], p[0x5], p[0x6], p[0x7],
                p[0x8], p[0x9], p[0xA], p[0xB],
                p[0xC], p[0xD], p[0xE], p[0xF],
                memchar(p[0x0]), memchar(p[0x1]),
                memchar(p[0x2]), memchar(p[0x3]),
                memchar(p[0x4]), memchar(p[0x5]),
                memchar(p[0x6]), memchar(p[0x7]),
                memchar(p[0x8]), memchar(p[0x9]),
                memchar(p[0xA]), memchar(p[0xB]),
                memchar(p[0xC]), memchar(p[0xD]),
                memchar(p[0xE]), memchar(p[0xF]));
        p += 16;
    }
}

uint32_t free_mem_addr = (uint32_t)&start;
/* Implementation is just a pointer to some free memory which
 * keeps growing */
uint32_t kmalloc(size_t size, int align, uint32_t *phys_addr) {
    // Pages are aligned to 4K, or 0x1000
    if (align == 1 && (free_mem_addr & 0x00000FFF)) {
        free_mem_addr &= 0xFFFFF000;
        free_mem_addr += 0x1000;
    }
    // Save also the physical address
    if (phys_addr) *phys_addr = free_mem_addr;

    uint32_t ret = free_mem_addr;
    free_mem_addr += size; // Remember to increment the pointer
    return ret;
}
