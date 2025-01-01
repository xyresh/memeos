#include "stdio.h"  // Include the header where enum is declared

#include <stdarg.h>

/* VGA constants */
static const size_t VGA_WIDTH = 80;
static const size_t VGA_HEIGHT = 25;

/* Terminal state */
size_t terminal_row;
size_t terminal_column;
uint8_t terminal_color;
uint16_t* terminal_buffer;

/* VGA Helpers */
static inline uint8_t vga_entry_color(enum vga_color fg, enum vga_color bg) {
    return fg | bg << 4;
}

static inline uint16_t vga_entry(unsigned char uc, uint8_t color) {
    return (uint16_t) uc | (uint16_t) color << 8;
}

/* String helpers */
size_t strlen(const char* str) {
    size_t len = 0;
    while (str[len])
        len++;
    return len;
}

/* Terminal functions */
void terminal_initialize(void) {
    terminal_row = 0;
    terminal_column = 0;
    terminal_color = vga_entry_color(VGA_COLOR_LIGHT_GREY, VGA_COLOR_BLACK);
    terminal_buffer = (uint16_t*) 0xB8000;
    for (size_t y = 0; y < VGA_HEIGHT; y++) {
        for (size_t x = 0; x < VGA_WIDTH; x++) {
            const size_t index = y * VGA_WIDTH + x;
            terminal_buffer[index] = vga_entry(' ', terminal_color);
        }
    }
}

void terminal_setcolor(uint8_t color) {
    terminal_color = color;
}

void terminal_putentryat(char c, uint8_t color, size_t x, size_t y) {
    const size_t index = y * VGA_WIDTH + x;
    terminal_buffer[index] = vga_entry(c, color);
}

void terminal_putchar(char c) {
    if (c == '\n') {
        terminal_row++;
        terminal_column = 0;
        return;
    }

    terminal_putentryat(c, terminal_color, terminal_column, terminal_row);
    if (++terminal_column == VGA_WIDTH) {
        terminal_column = 0;
        if (++terminal_row == VGA_HEIGHT)
            terminal_row = 0;  // Wrap around
    }
}

void terminal_write(const char* data, size_t size) {
    for (size_t i = 0; i < size; i++)
        terminal_putchar(data[i]);
}

void terminal_writestring(const char* data) {
    terminal_write(data, strlen(data));
}

/* New printf-like functionality */

/* Write a single digit */
void terminal_write_digit(int num) {
    terminal_putchar(num + '0');
}

/* Write an integer */
void terminal_write_int(int num) {
    if (num == 0) {
        terminal_putchar('0');
        return;
    }

    int divisor = 1000000000; // Max int is 10 digits
    int leading_zero = 1;

    while (divisor > 0) {
        int digit = num / divisor;
        if (digit != 0 || !leading_zero) {
            terminal_write_digit(digit);
            leading_zero = 0;
        }
        num %= divisor;
        divisor /= 10;
    }
}

/* Write a string */
void terminal_write_string(const char* str) {
    while (*str != '\0') {
        terminal_putchar(*str);
        str++;
    }
}

/* Simple printf implementation */
void terminal_printf(const char* format, ...) {
    va_list args;
    va_start(args, format);

    while (*format) {
        if (*format == '%') {
            format++;
            if (*format == 'd') {
                int num = va_arg(args, int);
                terminal_write_int(num);
            } else if (*format == 's') {
                char* str = va_arg(args, char*);
                terminal_write_string(str);
            } else if (*format == 'c') {
                char c = (char) va_arg(args, int);
                terminal_putchar(c);
            }
        } else {
            terminal_putchar(*format);
        }
        format++;
    }

    va_end(args);
}
