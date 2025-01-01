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

    terminal_writestring("Testing heap allocation...\n");

    void* ptr1 = kmalloc(64); // Allocate 64 bytes
    if (ptr1) {
        terminal_writestring("Allocated 64 bytes\n");
    } else {
        terminal_writestring("Allocation failed\n");
    }

    void* ptr2 = kmalloc(128); // Allocate 128 bytes
    if (ptr2) {
        terminal_writestring("Allocated 128 bytes\n");
    } else {
        terminal_writestring("Allocation failed\n");
    }

    kfree(ptr1); // Free the first block
    terminal_writestring("Freed 64 bytes\n");

    void* ptr3 = kmalloc(64); // Allocate again to test reuse
    if (ptr3) {
        terminal_writestring("Re-allocated 64 bytes\n");
    } else {
        terminal_writestring("Allocation failed\n");
    }

}
