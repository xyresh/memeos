#ifndef MEMORY_H
#define MEMORY_H

#include <stddef.h>
#include <stdint.h>

// Memory management functions
void memory_init(size_t size);
void* kmalloc(size_t size);
void kfree(void* ptr);
void memory_debug(void);
void memory_stats();

// External variables for heap management
extern uint8_t* heap;        // Base address of the heap
extern uint8_t* heap_ptr;    // Current position in the heap
extern size_t HEAP_SIZE;     // Total heap size

#endif // MEMORY_H
