#include <stdint.h>
#include <stddef.h>

#include "memory.h"
#include "stdio.h"

#define PAGE_SIZE 4096
#define BITMAP_SIZE 32768 // Example: Tracks up to 128 MB (128 * 1024 * 1024 / PAGE_SIZE)
#define ALIGNMENT 8  // Ensure 8-byte alignment
#define MIN_BLOCK_SIZE sizeof(block_header_t)

static uint8_t bitmap[BITMAP_SIZE];
static uintptr_t heap_top = 0xC0000000; // Example kernel heap start

static uint8_t static_heap[0x100000]; // Maximum heap size, adjust as needed
uint8_t* heap = NULL;
uint8_t* heap_ptr = NULL;
size_t HEAP_SIZE = 0;


typedef struct block_header {
    size_t size;             // Size of the allocated block
    struct block_header* next; // Pointer to the next free block
    int free;                // 1 if free, 0 if allocated
} block_header_t;

block_header_t* free_list = NULL; // Points to the first free block

void memory_init(size_t size) {
    if (heap != NULL) {
        terminal_writestring("Error: memory_init() called more than once.\n");
        return;
    }

    if (size == 0 || size > 0x100000) {
        terminal_writestring("Error: Invalid heap size specified.\n");
        return;
    }

    heap = (uint8_t*)static_heap; // Example static memory block
    HEAP_SIZE = size;
    heap_ptr = heap;

    // Initialize the free list
    free_list = (block_header_t*)heap;
    free_list->size = HEAP_SIZE;
    free_list->next = NULL;
    free_list->free = 1;

    terminal_writestring("Heap initialized successfully.\n");
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

size_t align_up(size_t size) {
    return (size + ALIGNMENT - 1) & ~(ALIGNMENT - 1);
}   

void* kmalloc(size_t size) {
    if (heap == NULL || heap_ptr == NULL) {
        terminal_writestring("Error: kmalloc() called before memory_init().\n");
        return NULL;
    }

    // Ensure the requested size is aligned
    size_t total_size = align_up(size) + sizeof(block_header_t);  // Align size and add header size
    block_header_t* current = free_list;
    block_header_t* previous = NULL;

    // First-fit search
    while (current) {
        if (current->free && current->size >= total_size) {
            size_t remaining_size = current->size - total_size;

            // Split the block if there's enough space left
            if (remaining_size >= sizeof(block_header_t) + MIN_BLOCK_SIZE) {
                block_header_t* new_block = (block_header_t*)((uint8_t*)current + total_size);
                new_block->size = remaining_size;
                new_block->free = 1;
                new_block->next = current->next;

                current->size = total_size;
                current->next = new_block;
            }

            current->free = 0;
            return (void*)((uint8_t*)current + sizeof(block_header_t));
        }
        previous = current;
        current = current->next;
    }

    // No suitable block found; allocate from heap
    if (size > (HEAP_SIZE - (size_t)(heap_ptr - heap))) {
        terminal_writestring("Error: Not enough memory for kmalloc().\n");
        return NULL;
    }

    // Create a new block at the heap pointer
    block_header_t* new_block = (block_header_t*)heap_ptr;
    new_block->size = total_size;
    new_block->free = 0;
    new_block->next = NULL;

    heap_ptr += total_size;  // Move heap pointer forward

    // No need to split if there's no leftover space
    if (previous) {
        previous->next = new_block;
    } else {
        free_list = new_block;  // Set as the first free block if it's the first allocation
    }

    return (void*)((uint8_t*)new_block + sizeof(block_header_t));
}

void* kmalloc_aligned(size_t size, size_t alignment) {
    if (alignment & (alignment - 1)) { // Check if alignment is a power of 2
        terminal_writestring("Error: Alignment must be a power of 2.\n");
        return NULL;
    }

    size_t total_size = size + alignment - 1 + sizeof(block_header_t);
    void* raw = kmalloc(total_size);
    if (!raw) return NULL;

    uintptr_t aligned_address = ((uintptr_t)raw + sizeof(block_header_t) + alignment - 1) & ~(alignment - 1);
    return (void*)aligned_address;
}

// Free function to release memory
void kfree(void* ptr) {
    if (ptr == NULL || ptr < (void*)heap || ptr >= (void*)(heap + HEAP_SIZE)) {
        terminal_writestring("Error: Invalid pointer passed to kfree.\n");
        return;
    }

    // Get the block header
    block_header_t* block = (block_header_t*)((uint8_t*)ptr - sizeof(block_header_t));
    if (block->free) {
        terminal_writestring("Error: Double free detected.\n");
        return;
    }

    // Mark the block as free
    block->free = 1;

    // Coalesce with next block if possible
    if (block->next && block->next->free) {
        block->size += block->next->size + sizeof(block_header_t);
        block->next = block->next->next;
    }

    // Traverse the free list to coalesce with the previous block
    block_header_t* current = free_list;
    while (current) {
        if (current->next == block && current->free) {
            current->size += block->size + sizeof(block_header_t);
            current->next = block->next;
            break;
        }
        current = current->next;
    }
}

void memory_stats() {
    size_t total_free = 0;
    size_t total_allocated = 0;
    size_t max_free_block = 0;
    size_t block_count = 0;

    block_header_t* current = free_list;
    while (current) {
        block_count++;
        if (current->free) {
            total_free += current->size;
            if (current->size > max_free_block) {
                max_free_block = current->size;
            }
        } else {
            total_allocated += current->size;
        }
        current = current->next;
    }

    terminal_writestring("Heap Statistics:\n");
    terminal_writestring("Total Allocated: ");
    terminal_write_int(total_allocated);
    terminal_writestring(" bytes\n");

    terminal_writestring("Total Free: ");
    
    terminal_write_int(total_free);
    terminal_writestring(" bytes\n");

    terminal_writestring("Max Free Block: ");
    terminal_write_int(max_free_block);
    terminal_writestring(" bytes\n");

    terminal_writestring("Number of Blocks: ");
    terminal_write_int(block_count);
    terminal_writestring("\n");
}



void memory_debug() {
    block_header_t* current = free_list;
    size_t total_free_space = 0;
    size_t total_used_space = 0;

    terminal_writestring("Free blocks:\n");
    while (current) {
        if (current->free) {
            terminal_write_hex((uintptr_t)current);
            terminal_writestring(" - Free block, Size: ");
            terminal_write_int(current->size);
            terminal_writestring("\n");
            total_free_space += current->size;
        } else {
            terminal_write_hex((uintptr_t)current);
            terminal_writestring(" - Used block, Size: ");
            terminal_write_int(current->size);
            terminal_writestring("\n");
            total_used_space += current->size;
        }
        current = current->next;
    }

    terminal_writestring("\nTotal Free Space: ");
    terminal_write_int(total_free_space);
    terminal_writestring("\nTotal Used Space: ");
    terminal_write_int(total_used_space);
    terminal_writestring("\n\n");
}