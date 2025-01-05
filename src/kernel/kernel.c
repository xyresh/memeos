#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

//use memeos headers
#include "memory.h"
#include "stdio.h"
#include "multitasking.h"


/* Check if the compiler thinks you are targeting the wrong operating system. */
#if defined(__linux__)
#error "You are not using a cross-compiler, you will most certainly run into trouble"
#endif

/* This tutorial will only work for the 32-bit ix86 targets. */
#if !defined(__i386__)
#error "This tutorial needs to be compiled with a ix86-elf compiler"
#endif
/*
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
*/


void task1(void) {
    while (1) {
        terminal_writestring("Task 1 is running...\n");
        for (volatile int i = 0; i < 1000000; i++) {} // A delay to avoid flooding the terminal too quickly
    }
}


void task2(void) {
    while (1) {
        terminal_writestring("Task 2 is running...\n");
        for (volatile int i = 0; i < 1000000; i++) {} // A delay to avoid flooding the terminal too quickly
    }
}

void task3(void) {
    while (1) {
        terminal_writestring("Task 3 is running...\n");
        for (volatile int i = 0; i < 1000000; i++) {} // A delay to avoid flooding the terminal too quickly
    }
}

void kernel_main(void) {
    // Initialize the terminal
    terminal_initialize();
    terminal_writestring("Kernel initialized.\n");

    // Initialize memory management
    memory_init(10240); // Initialize heap with 10KB
    terminal_writestring("Memory management initialized.\n");

    // Initialize multitasking
    multitasking_init();
    terminal_writestring("Multitasking initialized.\n");

    // Create tasks
    create_task(task1);
    create_task(task2);
    create_task(task3);
    create_task(idle_task);  // Add the idle task
    terminal_writestring("Tasks created.\n");

    // Set the current task to the first task in the list
    current_task = task_list;  // Ensure the scheduler starts from the first task
    terminal_writestring("Current task set to the first task.\n");

    // Simulate multitasking by invoking the scheduler manually
    while (1) {
        scheduler_tick();  // The scheduler will now pick tasks, including the idle task
        memory_stats();
    }
}
