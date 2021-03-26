#ifndef SERIAL_H
#define SERIAL_H

#include <stdint.h>

#define COM1 0x3F8

void write_serial_port(uint16_t PORT, char *word);
int serial_install_port(uint16_t PORT);
char read_serial(uint16_t PORT);
void qemu_printf(char *s, ...);
void write_serial(char *word);
void serial_install();

#endif
