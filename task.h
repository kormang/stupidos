#ifndef TASK_H
#define TASK_H
#include "common.h"
#include "paging.h"
#include "isr.h"

typedef struct task {
  int id; // PID
  registers_t registers;
  page_directory_t* page_directory;
  struct task* next; // next node in linked list
} __attribute__((packed)) task_t;
// it is packed for easier assemby code

void init_tasking();
void start_process();
void switch_task_from_isr(registers_t* interrupted_task_regs);
void schedule_from_timer(registers_t* interrupted_task_regs);
void switch_task();
int fork();
int get_pid();


#endif
