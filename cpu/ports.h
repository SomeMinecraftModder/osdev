#ifndef PORTS_H
#define PORTS_H

#include <stdint.h>

void port_data_out(uint16_t port, uint32_t data);
void port_word_out(uint16_t port, uint16_t data);
void port_byte_out(uint16_t port, uint8_t data);

uint8_t port_byte_in(uint16_t port);
uint16_t port_word_in(uint16_t port);
uint32_t port_data_in(uint16_t port);

void io_wait();

#endif
