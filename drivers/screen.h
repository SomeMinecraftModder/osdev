#ifndef FRAMEBUFFER_H
#define FRAMEBUFFER_H

#include "../kernel/multiboot.h"
#include "psf.h"
#include <stddef.h>
#include <stdint.h>

typedef struct {
    uint8_t r, g, b;
} Color;

extern multiboot_info_t *mbi;
extern Color white;
extern Color green;
extern Color black;
extern Color red;

void kprint_at(char *string, int position_x, int position_y, Color color);
void putchar_at(char c, int position_x, int position_y, Color color);
void kprint_color(char *string, Color color);
void putchar_color(char c, Color color);
void init_video(uint32_t addr);
Color rgb(int r, int g, int b);
void kprint(char *string);
void kprint_backspace();
void kprint_newline();
void putchar(char c);
void clear_screen();
void kprint_rfail();
void kprint_gok();

#endif
