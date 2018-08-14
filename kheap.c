#include "kheap.h"

extern uint32_t kernel_end; // defined in link.ld
void* kernel_heap_end = (void*)0;

void* placement_address = (void*)&kernel_end;

void* kmalloc_i(size_t size, int align, void** phys) {
  if (align && (void*)((uint32_t)placement_address & 0xFFF)) {
    // aligning pool if not aligned
    placement_address = (void*)((uint32_t)placement_address & ~0xFFF) + 0x1000;
  }
  if (phys) {
    *phys = placement_address;
  }
  void* ret = placement_address;
  placement_address += size;
  return ret;
}

void kfree(void* address) {
  // TODO: implement proper heap
}
