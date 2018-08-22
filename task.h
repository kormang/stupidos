#ifndef TASK_H
#define TASK_H
#include "common.h"
#include "paging.h"
#include "isr.h"

#define WAITING_KEYBOARD 1

typedef struct task {
  int id; // PID
  registers_t registers;
  page_directory_t* page_directory;
  struct task* next; // next node in linked list
  uint32_t waiting_input;
} __attribute__((packed)) task_t;
// it is packed for easier assemby code

extern volatile task_t* current_task;

void init_tasking();
void start_process();
void switch_task_from_isr(registers_t* interrupted_task_regs);
void schedule_from_timer(registers_t* interrupted_task_regs);
void switch_to_user_mode();
void on_task_returned();
int fork();
int fork_user(registers_t* reg);
task_t* find_task_waiting(uint32_t what);
int get_pid();


#endif
