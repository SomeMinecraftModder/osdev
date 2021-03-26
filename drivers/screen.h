#ifndef SCREEN_H
#define SCREEN_H

#define VIDEO_ADDRESS 0xb8000
#define MAX_ROWS 25
#define MAX_COLS 80
#define WHITE_ON_BLACK 0x0F
#define GREEN_ON_BLACK 0x0A
#define RED_ON_BLACK 0x04
#define RED_ON_WHITE 0xF4

// Screen I/O ports
#define REG_SCREEN_CTRL 0x3D4
#define REG_SCREEN_DATA 0x3D5

// Public kernel API
void kprint_at(char *message, int col, int row, int color);
void enable_cursor(int cursor_start, int cursor_end);
void move_cursor(int col, int row);
void kprint_dec(int message);
void kprint(char *message);
void kprint_backspace();
void disable_cursor();
void putchar(char c);
void clear_screen();
void kprint_rfail();
void kprint_gok();

#endif
