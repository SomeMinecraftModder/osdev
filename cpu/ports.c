#include "ports.h"

#ifdef __STRICT_ANSI__
    #define asm __asm__
#endif

// Read a byte from the specified port
uint8_t port_byte_in(uint16_t port) {
    uint8_t result;
    /* Inline assembler syntax
     * Notice how the source and destination registers are switched from NASM
     *
     * '"=a" (result)'; set '=' the C variable '(result)' to the value of
     * register e'a'x
     * '"d" (port)': map the C variable '(port)' into e'd'x register
     *
     * Inputs and outputs are separated by colons
     */
    asm("inb %%dx, %%al" : "=a"(result) : "d"(port));
    return result;
}

uint16_t port_word_in(uint16_t port) {
    uint16_t result;
    asm("inw %%dx, %%ax" : "=a"(result) : "d"(port));
    return result;
}

uint32_t port_long_in(uint16_t port) {
    uint32_t result;
    asm("inl %%dx, %%eax" : "=a"(result) : "d"(port));
    return result;
}

void port_byte_out(uint16_t port, uint8_t data) {
    /* Notice how here both registers are mapped to C variables and
     * nothing is returned, thus, no equals '=' in the asm syntax
     * However we see a comma since there are two variables in the input area
     * and none in the 'return' area
     */
    asm volatile("outb %%al, %%dx" : : "a"(data), "d"(port));
}

void port_word_out(uint16_t port, uint16_t data) {
    asm volatile("outw %%ax, %%dx" : : "a"(data), "d"(port));
}

void port_long_out(uint16_t port, uint32_t data) {
    asm volatile("outl %%eax, %%dx" : : "a"(data), "d"(port));
}

void io_wait() {
    port_byte_in(0x80);
    port_byte_in(0x80);
    port_byte_in(0x80);
    port_byte_in(0x80);
}
