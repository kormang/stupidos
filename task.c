#include "task.h"
#include "kheap.h"
#include "paging.h"
#include "screen.h"
#include "string.h"

// This must be outside kernel virtual space.
// 0xC0000000 is high enough and it is lower than typical higher-helf kernel address,
// in case this kernel ever becomes one.
// Stack grows downwards.
#define PROCESS_STACK_ADDRESS 0xC0000000 // at 3 GB into virt addr space
#define PROCESS_STACK_SIZE 0x20000 // 128 kB

#define USER_MODE_PROGRAM_ADDRESS 0x10000000 // at 256 MB into virt addr space

volatile task_t* current_task;
volatile task_t* task_queue;
volatile task_t* task_queue_end;

volatile task_t* next_task;

extern page_directory_t *kernel_directory;
extern page_directory_t *current_directory;

extern int read_eip();
extern uint32_t kernel_dispatcher_stack_start;

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
  // decrease by at least 4 so that esp points inside stack area
  // additional bytes are for debug and prevetion (when writting to regs->ss)
  process_stack_start -= 32;
  return process_stack_start;
}

extern uint8_t binary_program_start;
extern uint8_t binary_program_end;
static virt_addr_t alloc_user_mode_program() {
  virt_addr_t program_start = (virt_addr_t)USER_MODE_PROGRAM_ADDRESS;
  size_t binary_program_size = &binary_program_end - &binary_program_start;
  for (virt_addr_t addr = program_start;
      addr < program_start + binary_program_size;
      addr += 0x1000) {
    phys_addr_t frame = alloc_frame();
    map_page(frame, addr, 0x7);
  }
  return program_start;
}

static void prepare_user_mode_program() {
  virt_addr_t dst = alloc_user_mode_program();
  virt_addr_t src = &binary_program_start;
  size_t size = &binary_program_end - &binary_program_start;
  memcpy(dst, src, size);
}

static registers_t switch_task_dispatcher_regs;
static void switch_task_dispatcher() {
  extern void switch_task_dispatched_to_user(phys_addr_t next_dir, registers_t regs);
  extern void switch_task_dispatched_kernel(phys_addr_t next_dir, registers_t* regs);

  __asm__ volatile ("cli");
  // ensure we run on dispatcher stack
  __asm__ volatile ("mov %0, %%esp" :: "r"(&kernel_dispatcher_stack_start) : "esp");
  current_task = next_task;
  current_directory = next_task->page_directory;

  if (next_task->registers.cs == 0x1B) {
    switch_task_dispatched_to_user(current_directory->dir_phys_addr, current_task->registers);
  } else {
    switch_task_dispatched_kernel(current_directory->dir_phys_addr, (registers_t*)&current_task->registers);
  }
}

void init_tasking() {
  current_task = task_queue = task_queue_end = (task_t*)kmalloc(sizeof(task_t));
  current_task->id = next_pid++;
  memset((void*)&current_task->registers, 0, sizeof(registers_t));
  current_task->page_directory = current_directory;
  current_task->next = 0;
  current_task->waiting_input = 0;
  current_task->registers.cs = 0x8; // kernel code
  current_task->registers.ss = 0x10; // kernel data
  current_task->registers.ds = 0x10; // kernel data

  process_stack_start = alloc_process_stack();


  // initialize switch_task_dispatcher_regs
  memset(&switch_task_dispatcher_regs, 0, sizeof(registers_t));
  switch_task_dispatcher_regs.prev_esp = (uint32_t)&kernel_dispatcher_stack_start;
  switch_task_dispatcher_regs.eip = (uint32_t)&switch_task_dispatcher;
  switch_task_dispatcher_regs.cs = 0x8; // kernel code
  switch_task_dispatcher_regs.ss = 0x10; // kernel data
  switch_task_dispatcher_regs.ds = 0x10; // kernel data
  __asm__ volatile (
    "pushf \n"
    "pop %%ecx \n"
    "mov %%ecx, %0 \n"
    :
    "=r"(switch_task_dispatcher_regs.eflags)
    ::
    "ecx"
  );
  // other fields are not important
}

int process_main() {
  prepare_user_mode_program();
  switch_to_user_mode(USER_MODE_PROGRAM_ADDRESS);
  return 0x1;
}

static void set_next_task(task_t* ntask) {
  next_task = ntask;
}

static task_t* find_next_task(task_t* current_task) {
  // next_task is static (in global memory),
  // it will be preserved after return from ISR
  task_t* next_task = current_task->next;
  if (!next_task) {
    next_task = (task_t*)task_queue->next;
  }
  while (next_task && next_task->waiting_input) {
    next_task = next_task->next;
  }
  if (!next_task) {
    return (task_t*)task_queue;
  }
  return next_task;
}

task_t* find_task_waiting(uint32_t what) {
  // stupid way to find task waiting for some input
  task_t* next_task = (task_t*)task_queue;
  while (next_task && !(next_task->waiting_input & what)) {
      next_task = next_task->next;
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
  task_t* tmp = (task_t*)task_queue;
  task_t* prev = 0;

  while (tmp != task_queue_end && tmp != current_task) {
    prev = tmp;
    tmp = tmp->next;
  }

  if (tmp == current_task) { // it should be
    if (prev == 0) {
      // first task is current task
      if (tmp != task_queue_end && tmp->next) {
        task_queue = tmp->next;
      } else {
        screen_print("Task should not finish!");
      }
    } else {
      prev->next = tmp->next;
      if (tmp == task_queue_end) {
        task_queue_end = prev;
      }
    }

    set_next_task(find_next_task(tmp));
    kfree(tmp);
    switch_task_dispatcher();
  } else {
    // this should not happen
    screen_print("Task finished but wasn't in list.\n");
    __asm__ volatile ("hlt");
  }
}

int fork() {
  __asm__ volatile ("pushf");
  __asm__ volatile ("cli");
  uint32_t eip;
  task_t* new_task;
  task_t* parent_task = (task_t*)current_task;

  new_task = (task_t*)kmalloc(sizeof(task_t));
  memset(new_task, 0, sizeof(task_t));
  new_task->id = next_pid++;
  new_task->page_directory = clone_directory(current_directory);
  new_task->next = 0;
  new_task->waiting_input = 0;

  // save eflags
  __asm__ volatile (
    "pushf \n"
    "pop %0 \n"
    : "=m"(new_task->registers.eflags)
  );

  new_task->registers.eflags |= 0x202; // set I flag upon wakeup (+ reserved 1)
  new_task->registers.ds = current_task->registers.ds;
  new_task->registers.ss = current_task->registers.ss;
  new_task->registers.cs = current_task->registers.cs;

  // so we setup registers for new task, which will eventually be woken up
  // at eip with these values of registers
  __asm__ volatile (
    "mov %0, %%eax \n"
    "mov %%edi, 0(%%eax) \n"
    "mov %%esi, 4(%%eax) \n"
    "mov %%ebp, 8(%%eax) \n"
    "mov %%esp, 52(%%eax) \n" // we need prev_esp
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
    task_queue_end->next = new_task;
    task_queue_end = new_task;

    __asm__ volatile ("popf");
    return new_task->id;
  } else {
    return 0; // when we return to child from fork we return 0
  }
}

int fork_user(registers_t* regs) {
  task_t* new_task;
  new_task = (task_t*)kmalloc(sizeof(task_t));
  memset(new_task, 0, sizeof(task_t));
  new_task->id = next_pid++;
  new_task->page_directory = clone_directory(current_directory);
  new_task->next = 0;
  new_task->waiting_input = 0;

  // so we setup registers for new task, which will eventually be woken up
  // at eip with these values of registers
  new_task->registers = *regs;
  regs->eax = new_task->id; // parent wakes up with id as return value
  new_task->registers.eax = 0; // child wakes up with 0 as return value
  task_queue_end->next = new_task;
  task_queue_end = new_task;
  return new_task->id;
}


// it should be called from interrupt handler,
// it will return quickly, but will modify ISR return address,
// so that it points to second part of this function.
// This second part will efectivelly switch task but outside ISR
void switch_task_from_isr(registers_t* interrupted_task_regs) {
  if (!current_task || task_queue == task_queue_end) {
    return;
  }

  task_t* next = find_next_task((task_t*)current_task);
  if (next == current_task) {
    return;
  }

  current_task->registers = *interrupted_task_regs;

  if (current_task->registers.cs != 0x1B) {
    // kernel task interrupted

    // in this case prev_esp is not pushed automatically
    // we need to set prev_esp to saved isr_esp + some offset
    current_task->registers.prev_esp
      = current_task->registers.isr_esp
      + ( member_offset(registers_t, prev_esp) - member_offset(registers_t, int_no) );
  }

  set_next_task(next);

  interrupted_task_regs->eip = switch_task_dispatcher_regs.eip;
  interrupted_task_regs->eflags = switch_task_dispatcher_regs.eflags;

  if (current_task->registers.cs == 0x1B) {
    // user task is interrupted
    interrupted_task_regs->cs = switch_task_dispatcher_regs.cs;
    interrupted_task_regs->ds = switch_task_dispatcher_regs.ds;
    interrupted_task_regs->ss = switch_task_dispatcher_regs.ss;
    interrupted_task_regs->prev_esp = switch_task_dispatcher_regs.prev_esp;
  }

  // exit quickly from ISR, but continue with the job in switch_task_dispatched
  return;
}

int getpid() {
  return current_task->id;
}

void schedule_from_timer(registers_t* interrupted_task_regs) {
  // here goes advanced scheduling algorithm :)
  switch_task_from_isr(interrupted_task_regs);
}
