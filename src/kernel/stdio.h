#ifndef STDIO_H
#define STDIO_H

#include <stddef.h>
#include <stdint.h>

/* VGA Colors - defined once in the header */
enum vga_color {
    VGA_COLOR_BLACK,
    VGA_COLOR_BLUE,
    VGA_COLOR_GREEN,
    VGA_COLOR_CYAN,
    VGA_COLOR_RED,
    VGA_COLOR_MAGENTA,
    VGA_COLOR_BROWN,
    VGA_COLOR_LIGHT_GREY,
    VGA_COLOR_DARK_GREY,
    VGA_COLOR_LIGHT_BLUE,
    VGA_COLOR_LIGHT_GREEN,
    VGA_COLOR_LIGHT_CYAN,
    VGA_COLOR_LIGHT_RED,
    VGA_COLOR_LIGHT_MAGENTA,
    VGA_COLOR_LIGHT_BROWN,
    VGA_COLOR_WHITE,
};

/* Terminal functions */
void terminal_initialize(void);
void terminal_setcolor(uint8_t color);
void terminal_putchar(char c);
void terminal_write(const char* data, size_t size);
void terminal_writestring(const char* data);

/* New printf-like functionality */
void terminal_write_int(int num);
void terminal_write_string(const char* str);
void terminal_write_hex(uintptr_t num);
void terminal_printf(const char* format, ...);
void itoa(int num, char* str, int base);


#endif /* STDIO_H */
