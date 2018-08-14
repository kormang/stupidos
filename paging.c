#include "paging.h"
#include "screen.h"
#include "interrupts.h"
#include "string.h"
#include "kheap.h"

// code is taken from https://web.archive.org/web/20160326061042/http://jamesmolloy.co.uk/tutorial_html/6.-Paging.html
// and adopted


// A bitset of frames - used or free.
uint32_t *frames;
uint32_t frames_count;
page_directory_t* current_directory;
page_directory_t* kernel_directory;

// Defined in kheap.c
extern void* placement_address;

void map_page_in_dir (phys_addr_t, virt_addr_t, uint32_t, page_directory_t*);

// Macros used in the bitset algorithms.
#define INDEX_FROM_BIT(a) (a >> 5) // devided by 32 (each entry is 32 bits)
#define OFFSET_FROM_BIT(a) (a % 32)

// Static function to set a bit in the frames bitset
static void set_frame(uint32_t frame_addr) {
  uint32_t frame = frame_addr/0x1000;
  uint32_t idx = INDEX_FROM_BIT(frame);
  uint32_t off = OFFSET_FROM_BIT(frame);
  frames[idx] |= (0x1 << off);
}

// Static function to clear a bit in the frames bitset
static void clear_frame(uint32_t frame_addr) {
  uint32_t frame = frame_addr/0x1000;
  uint32_t idx = INDEX_FROM_BIT(frame);
  uint32_t off = OFFSET_FROM_BIT(frame);
  frames[idx] &= ~(0x1 << off);
}

// Static function to test if a bit is set.
// static uint32_t test_frame(uint32_t frame_addr) {
//   uint32_t frame = frame_addr/0x1000;
//   uint32_t idx = INDEX_FROM_BIT(frame);
//   uint32_t off = OFFSET_FROM_BIT(frame);
//   return (frames[idx] & (0x1 << off));
// }

// Static function to find the first free frame.
static uint32_t first_frame() {
  uint32_t i, j;
  for (i = 0; i < INDEX_FROM_BIT(frames_count); i++) {
    // if nothing is free, exit early.
    if (frames[i] != 0xFFFFFFFF) {
      // at least one bit is free here.
      for (j = 0; j < 32; j++) {
        uint32_t toTest = 0x1 << j;
        if ( !(frames[i]&toTest) ) {
          return i*4*8+j;
        }
      }
    }
  }
  return (uint32_t)-1;
}

// Function to allocate a frame.
phys_addr_t alloc_frame() {
  uint32_t idx = first_frame(); // idx is now the index of the first free frame.
  if (idx == (uint32_t)-1) {
     screen_print("No free frames!");
     __asm__ volatile ("hlt");
  }
  phys_addr_t addr = (phys_addr_t)(idx << 12);
  set_frame((uint32_t)addr); // this frame is now ours!
  return addr;
}

// Function to deallocate a frame.
void free_frame(page_t* page) {
  uint32_t frame;
  if (!(frame = page->frame)) {
    return; // The given page didn't actually have an allocated frame!
  } else {
    clear_frame(frame); // Frame is now free again.
  }
}

void switch_page_directory(page_directory_t* pd) {
  current_directory = pd;
  __asm__ volatile ("mov %0, %%cr3" : : "r"(pd->dir_phys_addr));
  uint32_t cr0;
  __asm__ volatile ("mov %%cr0, %0": "=r"(cr0):);
  cr0 |= 0x80000000;
  __asm__ volatile ("mov %0, %%cr0" : : "r"(cr0));
}

void page_fault(registers_t* regs) {
  // A page fault has occurred.
  // The faulting address is stored in the CR2 register.
  uint32_t faulting_address;
  asm volatile("mov %%cr2, %0" : "=r" (faulting_address));

  // The error code gives us details of what happened.
  // int present   = !(regs->err_code & 0x1); // Page not present
  // int rw = regs->err_code & 0x2;           // Write operation?
  // int us = regs->err_code & 0x4;           // Processor was in user-mode?
  // int reserved = regs->err_code & 0x8;     // Overwritten CPU-reserved bits of page entry?
  // int id = regs->err_code & 0x10;          // Caused by an instruction fetch?

  // Output an error message.
  screen_print("Page fault! (");
  screen_print_hex(regs->err_code);
  screen_print(") at ");
  screen_print_hex(faulting_address);
  screen_put_char('\n');
  __asm__ volatile ("hlt");
}

extern void copy_frame(phys_addr_t src, phys_addr_t dst);

static page_table_t* clone_table(page_table_t* src, phys_addr_t* physaddr) {
  page_table_t* clone = (page_table_t*)kmalloc_ap(sizeof(page_table_t), physaddr);
  memset(clone, 0, sizeof(page_table_t));
  int i;
  for (i = 0; i < 1024; ++i) {
    if (!src->pages[i].frame) {
      continue;
    }
    phys_addr_t frameaddr = alloc_frame();
    clone->pages[i].frame = (uint32_t)frameaddr >> 12;
    clone->pages[i].flags = src->pages[i].flags;
    copy_frame((phys_addr_t)(src->pages[i].frame << 12), frameaddr);
  }

  return clone;
}

page_directory_t* clone_directory(page_directory_t* src) {
  phys_addr_t phys;

  page_directory_t* clone =
    (page_directory_t*)kmalloc_ap(sizeof(page_directory_t), &phys);

  memset(clone, 0, sizeof(page_directory_t));

  clone->dir_phys_addr = phys;

  int i;
  for(i = 0; i < 1024; ++i) {
    if (!src->virt_tables[i]) {
      continue;
    }
    // if table belongs to kernel, just link it
    if (kernel_directory->virt_tables[i] == src->virt_tables[i]) {
      // just link kernel tables
      clone->virt_tables[i] = src->virt_tables[i];
      clone->page_tables[i] = src->page_tables[i];
    } else {
      // otherwise deep copy tables
      clone->virt_tables[i] = clone_table(src->virt_tables[i], &phys);
      clone->page_tables[i].flags = 0x7;
      clone->page_tables[i].page_table_addr = (uint32_t)phys >> 12;
    }
  }

  return clone;
}

void init_paging() {
  uint32_t mem_end_page = 0x8000000; // 128 MB
  frames_count = mem_end_page >> 12;
  frames = kmalloc_a(INDEX_FROM_BIT(frames_count));
  memset(frames, 0, INDEX_FROM_BIT(frames_count));
  kernel_directory = (page_directory_t*)kmalloc_a(sizeof(page_directory_t));

  memset(kernel_directory, 0, sizeof(page_directory_t));
  kernel_directory->dir_phys_addr = kernel_directory;

  // identity map kernel so it can continue to work from base 0x100000
  uint32_t flags = 0x3; // Kernel code is readable but not writeable from userspace.
  phys_addr_t physaddr = 0;
  while (physaddr < placement_address + ADDITIONAL_KERNEL_HEAP_SIZE) {
    physaddr = alloc_frame();
    map_page_in_dir(physaddr, (virt_addr_t)physaddr, flags, kernel_directory);
  }
  kmalloc_a(0); // align placement_address
  kernel_heap_end = (void*)(placement_address + ADDITIONAL_KERNEL_HEAP_SIZE);

  register_interrupt_handler(14, &page_fault);

  switch_page_directory(kernel_directory);

  page_directory_t* newpd = clone_directory(kernel_directory);
  switch_page_directory(newpd);
}

phys_addr_t virt_to_phys(virt_addr_t virtaddr) {
  virt_addr_struct_t vs = { .addr = virtaddr };
  page_directory_t* pd = current_directory;
  pd_entry_t* pde = &pd->page_tables[vs.pd_index];
  if (pde->present) {
    page_t* pte = &pd->virt_tables[vs.pd_index]->pages[vs.pt_index];
    if (pte->present) {
      return (phys_addr_t)((pte->frame << 12) | vs.offset);
    } else {
      return (phys_addr_t)0;
    }
  } else {
    return (phys_addr_t)0;
  }
}

// used before paging is initialized
void map_page_in_dir (
  phys_addr_t physaddr,
  virt_addr_t virtaddr,
  uint32_t flags,
  page_directory_t* pd) {

  virt_addr_struct_t vs = { .addr = virtaddr };
  pd_entry_t* pd_entry = &pd->page_tables[vs.pd_index];
  if (!pd_entry->present) {
    phys_addr_t ptphys;
    page_table_t* table = kmalloc_ap(sizeof(page_table_t), &ptphys);

    memset(table, 0, sizeof(page_table_t));
    pd_entry->flags |= ((flags & 0xFFF) | 1); // just use the same flags as for page itself
    pd_entry->page_table_addr = ((uint32_t)ptphys >> 12);
    pd->virt_tables[vs.pd_index] = table;
  }

  page_table_t* table_virt = pd->virt_tables[vs.pd_index];
  table_virt->pages[vs.pt_index].flags = ((flags & 0xFFF) | 1);
  table_virt->pages[vs.pt_index].frame = (uint32_t)physaddr >> 12;
}

// used after paging is initialized
void map_page (phys_addr_t physaddr, virt_addr_t virtaddr, uint32_t flags) {
  map_page_in_dir(physaddr, virtaddr, flags, current_directory);
  __asm__ volatile ("invlpg (%0)" ::"r" (virtaddr) : "memory");
}
