#ifndef KEYBOARD_H
#define KEYBOARD_H

#define BACKSPACE 0x66
#define CAPSLOCK  0x58
#define LSHIFT    0x12
#define ENTER     0x5A
#define TAB       0x0D

#define TAB_SIZE 4

#define SC_MAX 132

#include <stdint.h>

void keybdual_write(uint8_t cmd);
void keybcon_write(uint8_t cmd);
void keyb_write(uint8_t cmd);
uint8_t keybcon_read();
void init_keyboard();
uint8_t keyb_read();

#endif
