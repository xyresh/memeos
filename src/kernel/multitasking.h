#ifndef MULTITASKING_H
#define MULTITASKING_H

#include <stdint.h>

// Task states
#define TASK_READY       0
#define TASK_RUNNING     1
#define TASK_WAITING     2
#define TASK_TERMINATED  3

// Task structure
typedef struct task {
    uint32_t id;                // Unique identifier for the task
    uint32_t* stack_pointer;    // Pointer to the current stack top
    uint8_t state;              // Current state of the task
    struct task* next;          // Pointer to the next task in the task list
    uint32_t registers[8];      // Registers saved during context switch (EAX, EBX, etc.)
} task_t;

// Global variables for task management
extern task_t* current_task;    // Pointer to the currently running task
extern task_t* task_list;       // Pointer to the head of the task list
void panic(const char* msg);

// Function prototypes
void multitasking_init(void);           // Initialize the multitasking system
task_t* create_task(void (*entry_point)(void)); // Create a new task
void scheduler_tick(void);              // Trigger a scheduler tick
void idle_task(void);                   // Idle task to run when no other task is ready
void save_task_state(task_t* task);     // Save the current task's CPU state
void restore_task_state(task_t* task);  // Restore the next task's CPU state

#endif // MULTITASKING_H
