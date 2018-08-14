#include "task.h"
#include "kheap.h"
#include "paging.h"
#include "screen.h"
#include "string.h"

// This must be outside kernel virtual space.
// 0xC0000000 is high enough and it is lower than typical higher-helf kernel address,
// in case this kernel ever becomes one.
// Stack grows downwards.
#define PROCESS_STACK_ADDRESS 0xC0000000
#define PROCESS_STACK_SIZE 0x20000 // 128 kB

volatile task_t* current_task;
volatile task_t* ready_queue;
volatile task_t* ready_queue_end;

volatile task_t* next_task;
volatile page_directory_t* next_directory;
phys_addr_t next_task_dir_phys_addr;
uint32_t next_task_esp;

extern page_directory_t *kernel_directory;
extern page_directory_t *current_directory;

extern int read_eip();
extern void switch_task_dispatched();

uint32_t next_pid = 1;
virt_addr_t process_stack_start = (virt_addr_t)0;

static virt_addr_t alloc_process_stack() {
  virt_addr_t process_stack_start = (virt_addr_t)PROCESS_STACK_ADDRESS;
  for (virt_addr_t addr = process_stack_start - PROCESS_STACK_SIZE;
      addr < process_stack_start;
      addr += 0x1000) {
    phys_addr_t frame = alloc_frame();
    map_page(frame, addr, 0x7);
  }
  process_stack_start -= 4;
  return process_stack_start;
}

void init_tasking() {
  __asm__ volatile ("cli");

  current_task = ready_queue = ready_queue_end = (task_t*)kmalloc(sizeof(task_t));
  current_task->id = next_pid++;
  memset((void*)&current_task->registers, 0, sizeof(registers_t));
  current_task->page_directory = current_directory;
  current_task->next = 0;

  process_stack_start = alloc_process_stack();

  __asm__ volatile ("sti");
}

int process_main() {
  for (int i = 0; i < 6; i++) {
		screen_print("Hello from another process!!!");
		screen_print("This is multitasking demo!!! This is one process!!!\n");
	}
  return 0x1; // error code
}

static void set_next_task(task_t* ntask) {
  next_task = ntask;
  // help for switch_task_dispatched assemby code
  next_directory = next_task->page_directory;
  next_task_dir_phys_addr = next_directory->dir_phys_addr;
  next_task_esp = next_task->registers.esp;
}

static task_t* find_next_task(task_t* current_task) {
  // next_task is static (in global memory),
  // it will be preserved after return from ISR
  task_t* next_task = current_task->next;
  if (!next_task) {
    next_task = (task_t*)ready_queue;
  }
  return next_task;
}

void on_task_returned() {
  uint32_t return_value;
  __asm__ volatile ("cli");
  __asm__ volatile ("mov %%eax, %0" : "=r"(return_value));
  screen_print("Task returned: ");
  screen_print_hex(return_value);
  screen_put_char('\n');
  task_t* tmp = (task_t*)ready_queue;
  task_t* prev = 0;

  while (tmp != ready_queue_end && tmp != current_task) {
    prev = tmp;
    tmp = tmp->next;
  }

  if (tmp == current_task) { // it should be
    if (prev == 0) {
      // first task is current task
      if (tmp != ready_queue_end && tmp->next) {
        ready_queue = tmp->next;
      } else {
        screen_print("Task should not finish!");
      }
    } else {
      prev->next = tmp->next;
      if (tmp == ready_queue_end) {
        ready_queue_end = prev;
      }
    }

    set_next_task(find_next_task(tmp));
    kfree(tmp);
    screen_print("process is finished\n");
    switch_task_dispatched();
  } else {
    // this should not happen
    screen_print("Task finished but wasn't in list.\n");
    __asm__ volatile ("hlt");
  }
}

int fork() {
  __asm__ volatile ("cli");
  uint32_t eip;
  task_t* new_task;
  task_t* parent_task = (task_t*)current_task;

  new_task = (task_t*)kmalloc(sizeof(task_t));
  memset(new_task, 0, sizeof(task_t));
  new_task->id = next_pid++;
  new_task->page_directory = clone_directory(current_directory);
  new_task->next = 0;

  // save eflags
  __asm__ volatile (
    "pushf \n"
    "pop %0 \n"
    : "=m"(new_task->registers.eflags)
  );

  // so we setup registers for new task, which will eventually be woken up
  // at eip with these values of registers
  __asm__ volatile (
    "mov %0, %%eax \n"
    "mov %%edi, 0(%%eax) \n"
    "mov %%esi, 4(%%eax) \n"
    "mov %%ebp, 8(%%eax) \n"
    "mov %%esp, 12(%%eax) \n"
    "mov %%ebx, 16(%%eax) \n"
    "mov %%edx, 20(%%eax) \n"
    "mov %%ecx, 24(%%eax) \n"
    "mov %%eax, 28(%%eax) \n"
    :: "r"(&new_task->registers.edi)
    : "memory"
  );

  // get the entry point
  eip = read_eip(); // genius way to read it :)

  if (current_task == parent_task) {
    new_task->registers.eip = eip;
    ready_queue_end->next = new_task;
    ready_queue_end = new_task;

    __asm__ volatile ("sti");

    return new_task->id;
  } else {
    return 0; // when we return to child from fork we return 0
  }
}

// it should be called from interrupt handler,
// it will return quickly, but will modify ISR return address,
// so that it points to second part of this function.
// This second part will efectivelly switch task but outside ISR
void switch_task_from_isr(registers_t* interrupted_task_regs) {
  if (!current_task || ready_queue == ready_queue_end) {
    return;
  }

  current_task->registers = *interrupted_task_regs;

  set_next_task(find_next_task((task_t*)current_task));

  interrupted_task_regs->eip = (uint32_t)&switch_task_dispatched;
  // exit quickly from ISR, but continue with the job in switch_task_dispatched
  return;
}

void switch_task() {
  set_next_task(find_next_task((task_t*)current_task));
  switch_task_dispatched();
}

int getpid() {
  return current_task->id;
}

void schedule_from_timer(registers_t* interrupted_task_regs) {
  // here goes advanced scheduling algorithm :)
  switch_task_from_isr(interrupted_task_regs);
}
