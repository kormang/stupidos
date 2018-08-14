#ifndef KHEAP_H
#define KHEAP_H
#include "common.h"
#define ADDITIONAL_KERNEL_HEAP_SIZE 0x100000

extern void* kernel_heap_end;

void* kmalloc_i(size_t size, int align, void** phys);

static inline void* kmalloc_a(size_t size) {
  return kmalloc_i(size, 1, (void**)0);
}

static inline void* kmalloc_p(size_t size, void** phys) {
  return kmalloc_i(size, 0, phys);
}

static inline void* kmalloc_ap(size_t size, void** phys) {
  return kmalloc_i(size, 1, phys);
}

static inline void* kmalloc(size_t size) {
  return kmalloc_i(size, 0, (void**)0);
}

void kfree(void* address);

#endif
