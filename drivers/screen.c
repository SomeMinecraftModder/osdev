#include "screen.h"
#include "../libc/string.h"

multiboot_info_t *mbi;

extern PSF fb_font;

Color white = {255, 255, 255};
Color green = {0, 255, 0};
Color red = {168, 0, 0};
Color black = {1, 1, 1};

int cursor_x = 0, cursor_y = 0;
uint32_t *fb;

void init_video(uint32_t addr) {
    mbi = (multiboot_info_t *)addr;
    fb = (uint32_t *)(uintptr_t)mbi->framebuffer_addr;
    clear_screen(black);
}

Color rgb(int r, int g, int b) {
    return (Color){r, g, b};
}

uint32_t get_color(Color *color) {
    return ((color->r & 0xFF) << 16) + ((color->g & 0xFF) << 8) +
           (color->b & 0xFF);
}

void draw_pixel(int x, int y, uint32_t color) {
    size_t fb_i = x + (mbi->framebuffer_pitch / sizeof(uint32_t)) * y;

    fb[fb_i] = color;
}

void putchar_at(char c, int position_x, int position_y, Color color) {
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

        case '\b':
            cursor_x--;
            return;
    }

    uint8_t *glyph = &fb_font.data[c * fb_font.glyph_size];

    if ((cursor_y * fb_font.height) >= mbi->framebuffer_height) {
        kprint_newline();
        putchar_at(c, cursor_x, cursor_y, color);
        cursor_x--;
    }

    size_t x = position_x * fb_font.width, y = position_y * fb_font.height;

    static const uint8_t masks[8] = {128, 64, 32, 16, 8, 4, 2, 1};

    size_t i, j;
    for (i = 0; i < fb_font.height; i++) {
        for (j = 0; j < fb_font.width; j++) {
            if (glyph[i] & masks[j]) {
                draw_pixel(x + j, y + i, get_color(&color));
            } else {
                draw_pixel(x + j, y + i, get_color(&black));
            }
        }
    }

    if (((cursor_x * fb_font.width) + (2 * fb_font.width)) >=
        mbi->framebuffer_width - (2 * fb_font.width)) {
        cursor_y++;
        cursor_x = 0;
    }

    if (c != '\n') {
        cursor_x++;
    }
}

void kprint_backspace() {
    if (cursor_x <= 1) {
        cursor_y--;
        /* 131 - 1 (See down) = 130
         * NOTE: This may broke when using other fonts */
        cursor_x = 131;
    }

    size_t x = (cursor_x * fb_font.width) - fb_font.width,
           y = cursor_y * fb_font.height;

    size_t i, j;
    for (i = 0; i < fb_font.height; i++) {
        for (j = 0; j < fb_font.width; j++) {
            draw_pixel(x + j, y + i, get_color(&black));
        }
    }

    cursor_x--;
}

void kprint_newline() {
    for (uint32_t y = fb_font.height; y != mbi->framebuffer_height; ++y) {
        void *dest = (void *)(((uintptr_t)fb) +
                              (y - fb_font.height) * mbi->framebuffer_pitch);
        const void *src =
          (void *)(((uintptr_t)fb) + y * mbi->framebuffer_pitch);
        memcpy(dest, src, mbi->framebuffer_width * 4);
    }

    cursor_y--;

    cursor_x = 131;
    size_t x = (cursor_x * fb_font.width) - fb_font.width,
           y = cursor_y * fb_font.height;

    size_t i, j;
    while (x >= (2 * fb_font.width)) {
        for (i = 0; i < fb_font.height; i++) {
            for (j = 0; j < fb_font.width; j++) {
                draw_pixel(x + j, y + i, get_color(&black));
            }
        }
        x = (cursor_x * fb_font.width) - fb_font.width,
        y = cursor_y * fb_font.height;
        cursor_x--;
    }
}

void putchar_color(char c, Color color) {
    putchar_at(c, cursor_x, cursor_y, color);
}

void putchar(char c) {
    putchar_color(c, white);
}

void kprint_at(char *string, int position_x, int position_y, Color color) {
    while (*string) {
        putchar_at(*string++, position_x, position_y, color);
        position_x++;
        cursor_x--;
    }
}

void kprint_color(char *string, Color color) {
    while (*string) {
        putchar_color(*string++, color);
    }
}

void kprint(char *string) {
    kprint_color(string, white);
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
