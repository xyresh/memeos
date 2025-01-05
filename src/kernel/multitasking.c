#include "multitasking.h"
#include "memory.h"
#include "stdio.h"

// Incremental task ID for uniquely identifying tasks
static uint32_t next_task_id = 1;

// Pointers to manage tasks
task_t* current_task = NULL;
task_t* task_list = NULL;

// Idle task structure
static task_t idle_task_struct;

// Idle task implementation
void idle_task(void) {
    while (1) {
        // Halt the CPU to save power and avoid unnecessary execution
        __asm__ volatile("hlt");
    }
}

void panic(const char* msg) {
    terminal_writestring("Kernel Panic: ");
    terminal_writestring(msg);
    while (1) { __asm__ volatile("hlt"); } // Halt the CPU
}


// Initialize multitasking
void multitasking_init(void) {
    terminal_writestring("Initializing multitasking...\n");

    // Create the idle task
    idle_task_struct.id = next_task_id++;
    idle_task_struct.stack_pointer = (uint32_t*)kmalloc(1024); // Allocate 1 KB stack
    idle_task_struct.state = TASK_READY;
    idle_task_struct.next = NULL;

    if (!idle_task_struct.stack_pointer) {
        panic("Failed to allocate stack for idle task.");
    }

    // Set up the initial stack for the idle task
    uint32_t* stack_top = idle_task_struct.stack_pointer + 1024 / sizeof(uint32_t);
    *(--stack_top) = (uint32_t)idle_task;   // Entry point address
    *(--stack_top) = 0x10;                  // Initial EFLAGS
    *(--stack_top) = 0;                     // Dummy return address
    idle_task_struct.stack_pointer = stack_top;

    // Add the idle task to the task list
    task_list = &idle_task_struct;

    // Set the current task to the idle task
    current_task = &idle_task_struct;

    terminal_writestring("Idle task created.\n");
}

// Create a new task
task_t* create_task(void (*entry_point)(void)) {
    task_t* new_task = (task_t*)kmalloc(sizeof(task_t));
    if (!new_task) {
        terminal_writestring("Error: Failed to allocate memory for new task.\n");
        return NULL;
    }

    // Initialize the task structure
    new_task->id = next_task_id++;
    new_task->stack_pointer = (uint32_t*)kmalloc(1024); // Allocate 1 KB stack
    new_task->state = TASK_READY;
    new_task->next = NULL;

    if (!new_task->stack_pointer) {
        terminal_writestring("Error: Failed to allocate stack for new task.\n");
        kfree(new_task);
        return NULL;
    }

    // Set up the initial stack
    uint32_t* stack_top = new_task->stack_pointer + 1024 / sizeof(uint32_t);
    *(--stack_top) = (uint32_t)entry_point; // Entry point address
    *(--stack_top) = 0x10;                  // Initial EFLAGS
    *(--stack_top) = 0;                     // Dummy return address
    new_task->stack_pointer = stack_top;

    // Add the task to the task list
    if (!task_list) {
        task_list = new_task;
    } else {
        task_t* temp = task_list;
        while (temp->next) temp = temp->next;
        temp->next = new_task;
    }

    terminal_writestring("New task created.\n");
    return new_task;
}

// Task scheduler
void scheduler_tick(void) {
    if (!current_task || !task_list) {
        terminal_writestring("No tasks to schedule.\n");
        return;
    }

    // Save the current task's state
    save_task_state(current_task);

    // Find the next READY task
    task_t* next_task = current_task->next;
    while (next_task && next_task->state != TASK_READY) {
        next_task = next_task->next;
    }

    // If no READY task is found, fall back to the idle task
    if (!next_task) {
        next_task = &idle_task_struct;
    }

    // Switch to the selected task
    current_task = next_task;
    current_task->state = TASK_RUNNING;
    restore_task_state(current_task);
}

// Save the task state (dummy implementation)
void save_task_state(task_t* task) {
    asm volatile (
        "movl %%eax, %0\n"
        "movl %%ebx, %1\n"
        "movl %%ecx, %2\n"
        "movl %%edx, %3\n"
        "movl %%esi, %4\n"
        "movl %%edi, %5\n"
        "movl %%ebp, %6\n"
        "movl %%esp, %7\n"
        : "=m"(task->registers[0]), "=m"(task->registers[1]),
          "=m"(task->registers[2]), "=m"(task->registers[3]),
          "=m"(task->registers[4]), "=m"(task->registers[5]),
          "=m"(task->registers[6]), "=m"(task->stack_pointer)
    );
}

// Restore the task state (dummy implementation)
void restore_task_state(task_t* task) {
    asm volatile (
        "movl %0, %%eax\n"
        "movl %1, %%ebx\n"
        "movl %2, %%ecx\n"
        "movl %3, %%edx\n"
        "movl %4, %%esi\n"
        "movl %5, %%edi\n"
        "movl %6, %%ebp\n"
        "movl %7, %%esp\n"
        :
        : "m"(task->registers[0]), "m"(task->registers[1]),
          "m"(task->registers[2]), "m"(task->registers[3]),
          "m"(task->registers[4]), "m"(task->registers[5]),
          "m"(task->registers[6]), "m"(task->stack_pointer)
    );
}
