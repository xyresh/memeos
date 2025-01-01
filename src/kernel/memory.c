#include <stdint.h>
#include <stddef.h>
#include "memory.h"

#define PAGE_SIZE 4096
#define BITMAP_SIZE 32768 // Example: Tracks up to 128 MB (128 * 1024 * 1024 / PAGE_SIZE)

static uint8_t bitmap[BITMAP_SIZE];
static uintptr_t heap_top = 0xC0000000; // Example kernel heap start

// Initialize physical memory manager
void memory_init(size_t memory_size) {
    size_t total_pages = memory_size / PAGE_SIZE;

    // Mark all pages as free
    for (size_t i = 0; i < BITMAP_SIZE; i++) {
        bitmap[i] = 0;
    }

    // Reserve kernel space (e.g., first N pages)
    for (size_t i = 0; i < (heap_top / PAGE_SIZE); i++) {
        bitmap[i / 8] |= (1 << (i % 8));
    }
}

// Allocate a physical page
void* alloc_page() {
    for (size_t i = 0; i < BITMAP_SIZE * 8; i++) {
        if (!(bitmap[i / 8] & (1 << (i % 8)))) {
            bitmap[i / 8] |= (1 << (i % 8));
            return (void*)(i * PAGE_SIZE);
        }
    }
    return NULL; // Out of memory
}

// Free a physical page
void free_page(void* addr) {
    size_t index = (uintptr_t)addr / PAGE_SIZE;
    bitmap[index / 8] &= ~(1 << (index % 8));
}

// Kernel heap allocation
void* kmalloc(size_t size) {
    void* addr = (void*)heap_top;
    heap_top += size;
    return addr;
}

void kfree(void* ptr) {
    // Simple allocator; doesn't yet support freeing
}