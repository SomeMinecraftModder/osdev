#include "mem.h"

void *memcpy(void *dest, const void *src, size_t nbytes) {
    uint8_t *q = (uint8_t *)dest;
    uint8_t *p = (uint8_t *)src;
    uint8_t *end = p + nbytes;

    while (p != end) {
        *q++ = *p++;
    }

    return dest;
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
    uint8_t *p = (uint8_t *)buf;
    uint8_t *end = p + n;

    while (p != end) {
        if (*p == c) {
            return p;
        }

        ++p;
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


/* This should be computed at link time, but a hardcoded
 * value is fine for now. Remember that our kernel starts
 * at 0x1000 as defined on the Makefile */
uint32_t free_mem_addr = 0x10000;
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
