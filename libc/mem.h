#ifndef MEM_H
#define MEM_H

#include <stdint.h>
#include <stddef.h>

void *memmove(void *dest, const void *src, size_t nbytes);
void *memcpy(void *dest, const void *src, size_t nbytes);
int memcmp(const void *s1, const void *s2, size_t n);
void *memset(void *dest, uint8_t val, size_t len);
void *memchr(const void *buf, int c, size_t n);

// At this stage there is no 'free' implemented.
uint32_t kmalloc(size_t size, int align, uint32_t *phys_addr);

#endif
