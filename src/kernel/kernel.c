#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

//use memeos headers
#include "memory.h"
#include "stdio.h"


/* Check if the compiler thinks you are targeting the wrong operating system. */
#if defined(__linux__)
#error "You are not using a cross-compiler, you will most certainly run into trouble"
#endif

/* This tutorial will only work for the 32-bit ix86 targets. */
#if !defined(__i386__)
#error "This tutorial needs to be compiled with a ix86-elf compiler"
#endif

void kernel_main(void) {
    terminal_initialize();

    memory_init(2048); // Initialize heap with 2048 bytes

    terminal_writestring("Testing heap allocation...\n");

    terminal_writestring("Initial heap pointer: ");
    terminal_write_hex((uintptr_t)heap_ptr);  // Display heap_ptr in hex
    terminal_writestring("\n");

    void* ptr1 = kmalloc(256);
    if (ptr1) {
        terminal_writestring("Allocated 256 bytes\n");
    }
    terminal_writestring("Current heap pointer: ");
    terminal_write_hex((uintptr_t)heap_ptr);  // Display heap_ptr after first allocation
    terminal_writestring("\n");

    memory_debug();

    void* ptr2 = kmalloc(256);
    if (ptr2) {
        terminal_writestring("Allocated 256 bytes\n");
    }
    terminal_writestring("Current heap pointer: ");
    terminal_write_hex((uintptr_t)heap_ptr);  // Display heap_ptr after second allocation
    terminal_writestring("\n");

    memory_debug();


    terminal_writestring("\n\n\n\n\n");
    memory_stats();
}
