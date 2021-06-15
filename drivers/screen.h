#ifndef FRAMEBUFFER_H
#define FRAMEBUFFER_H

#include "../kernel/multiboot.h"
#include <stddef.h>
#include <stdint.h>

typedef struct {
    uint8_t r, g, b;
} Color;

extern multiboot_info_t *mbi;
extern Color bgcolor;
extern Color white;
extern Color green;
extern Color black;
extern Color blue;
extern Color red;

#define rgb(r, g, b) \
    (Color) { r, g, b }

void kprint_at(char *string, int position_x, int position_y, Color color);
void putchar_at(int c, int position_x, int position_y, Color color);
void kprint_color(char *string, Color color);
void putchar_color(int c, Color color);
void init_video(uint32_t addr);
void kprint(char *string);
void kprint_backspace();
void kprint_newline();
void putchar(int c);
void clear_screen();
void kprint_rfail();
void kprint_gok();

#endif
