#ifndef MEMORY_H
#define MEMORY_H

#include <stddef.h>
#include <stdint.h>

void memory_init(size_t memory_size);
void* alloc_page();
void free_page(void* addr);
void* kmalloc(size_t size);
void kfree(void* ptr);

#endif // MEMORY_H