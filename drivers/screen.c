#include "screen.h"
#define SSFN_CONSOLEBITMAP_TRUECOLOR
#include "ssfn.h"
#include <string.h>

multiboot_info_t *mbi;

Color white = rgb(255, 255, 255);
Color green = rgb(0, 255, 0);
Color blue = rgb(0, 0, 255);
Color red = rgb(168, 0, 0);
Color black = rgb(1, 1, 1);

// Default color, can be changed by "clear_screen", used as text background
Color bgcolor = rgb(1, 1, 1);

int cursor_x = 0, cursor_y = 0;
uint32_t *fb;

extern unsigned char _binary_font_sfn_start;

void init_video(uint32_t addr) {
    mbi = (multiboot_info_t *)addr;
    fb = (uint32_t *)(uintptr_t)mbi->framebuffer_addr;

    ssfn_src = (ssfn_font_t *)&_binary_font_sfn_start;

    ssfn_dst.ptr = (uint8_t *)(uintptr_t)mbi->framebuffer_addr;
    ssfn_dst.w = mbi->framebuffer_width;
    ssfn_dst.h = mbi->framebuffer_height;
    ssfn_dst.p = mbi->framebuffer_pitch;
    ssfn_dst.x = ssfn_dst.y = 0;

    clear_screen(bgcolor);
}

uint32_t get_color(Color *color) {
    return ((color->r & 0xFF) << 16) + ((color->g & 0xFF) << 8) +
           (color->b & 0xFF);
}

void draw_pixel(int x, int y, uint32_t color) {
    size_t fb_i = x + (mbi->framebuffer_pitch / sizeof(uint32_t)) * y;

    fb[fb_i] = color;
}

void putchar_at(int c, int position_x, int position_y, Color color) {
    switch (c) {
        case '\n':
            cursor_y++;
            cursor_x = 1;
            return;

        case '\r':
            cursor_x = 1;
            return;

        case '\t':
            cursor_x += 4;
            return;

        case '\v':
            cursor_y++;
            cursor_x++;
            return;

        case '\b':
            cursor_x--;
            return;
    }

    if ((cursor_y * ssfn_src->height) >= (signed)mbi->framebuffer_height) {
        kprint_newline();
        putchar_at(c, cursor_x, cursor_y, color);
        cursor_x--;
    }

    if (ssfn_dst.fg != get_color(&color)) {
        ssfn_dst.fg = get_color(&color);
    }

    ssfn_dst.x = position_x * ssfn_src->width;
    ssfn_dst.y = position_y * ssfn_src->height;

    ssfn_putc(c);

    if (((cursor_x * ssfn_src->width) + (ssfn_src->width)) >=
        (signed)(mbi->framebuffer_width - (ssfn_src->width * 2))) {
        cursor_y++;
        cursor_x = 1;
        return;
    }

    if (c != '\n') {
        cursor_x++;
    }
}

void putchar_color(int c, Color color) {
    putchar_at(c, cursor_x, cursor_y, color);
}

void putchar(int c) {
    putchar_color(c, white);
}

void kprint_at(char *string, int position_x, int position_y, Color color) {
    while (*string) {
        putchar_at(ssfn_utf8(&string), position_x, position_y, color);
        position_x++;
        cursor_x--;
    }
}

void kprint_color(char *string, Color color) {
    while (*string) {
        putchar_color(ssfn_utf8(&string), color);
    }
}

void kprint(char *string) {
    kprint_color(string, white);
}

void kprint_backspace() {
    if (cursor_x <= 1) {
        cursor_y--;
        // NOTE: This may broke when using other fonts
        cursor_x = 132;
    }

    size_t x = (cursor_x * ssfn_src->width) - ssfn_src->width,
           y = cursor_y * ssfn_src->height;

    size_t i, j;
    for (i = 0; i < ssfn_src->height; i++) {
        for (j = 0; j < ssfn_src->width; j++) {
            draw_pixel(x + j, y + i, get_color(&bgcolor));
        }
    }

    cursor_x--;
}

void kprint_newline() {
    for (uint32_t y = ssfn_src->height; y != mbi->framebuffer_height; ++y) {
        void *dest = (void *)(((uintptr_t)fb) +
                              (y - ssfn_src->height) * mbi->framebuffer_pitch);
        const void *src =
          (void *)(((uintptr_t)fb) + y * mbi->framebuffer_pitch);
        memcpy(dest, src, mbi->framebuffer_width * 4);
    }

    cursor_y--;
    cursor_x = 132;

    size_t x = (cursor_x * ssfn_src->width) - ssfn_src->width,
           y = cursor_y * ssfn_src->height;

    size_t i, j;
    while (x >= (2 * ssfn_src->width)) {
        for (i = 0; i < ssfn_src->height; i++) {
            for (j = 0; j < ssfn_src->width; j++) {
                draw_pixel(x + j, y + i, get_color(&bgcolor));
            }
        }

        x = (cursor_x * ssfn_src->width) - ssfn_src->width,
        y = cursor_y * ssfn_src->height;
        cursor_x--;
    }
}

void clear_screen(Color color) {
    cursor_x = 1;
    cursor_y = 0;

    size_t x;
    size_t y;

    for (x = 0; x < mbi->framebuffer_width; x++) {
        for (y = 0; y < mbi->framebuffer_height; y++) {
            draw_pixel(x, y, get_color(&color));
        }
    }

    bgcolor = color;

    ssfn_dst.bg = get_color(&bgcolor);
}

void kprint_gok() {
    kprint("[ ");
    kprint_color("OK", green);
    kprint(" ] ");
}

void kprint_rfail() {
    kprint("[ ");
    kprint_color("FAIL", red);
    kprint(" ] ");
}
