#include "gdt.h"
#include "string.h"
#include "screen.h"

#define GDT_ENTRIES_COUNT 6
gdt_entry_t gdt_entries[GDT_ENTRIES_COUNT];
gdt_ptr_t gdt_ptr;
tss_entry_t tss_entry;

static void gdt_set_entry(int idx, uint32_t base, uint32_t limit, uint8_t access, uint8_t gran) {
   gdt_entries[idx].base_low    = (base & 0xFFFF);
   gdt_entries[idx].base_middle = (base >> 16) & 0xFF;
   gdt_entries[idx].base_high   = (base >> 24) & 0xFF;

   gdt_entries[idx].limit_low   = (limit & 0xFFFF);
   gdt_entries[idx].granularity = (limit >> 16) & 0x0F;

   gdt_entries[idx].granularity |= gran & 0xF0;
   gdt_entries[idx].access      = access;
}

static void write_tss(int idx, uint16_t ss0, uint16_t esp0) {
  uint32_t base = (uint32_t)&tss_entry;
  uint32_t limit = base + sizeof(tss_entry);
  gdt_set_entry(idx, base, limit, 0xE9, 0x00);
  memset(&tss_entry, 0, sizeof(tss_entry));

  // this will represent kernel stack
  tss_entry.ss0 = ss0;
  tss_entry.esp0 = esp0;
  tss_entry.cs = 0x08 | 0x3; // kernel code segment switchable to from ring 3
  tss_entry.ss = tss_entry.ds =
    tss_entry.es = tss_entry.fs = tss_entry.gs = 0x10 | 0x3;
}

void set_kernel_stack(uint32_t esp0) {
  tss_entry.esp0 = esp0;
}

extern void load_gdtr(gdt_ptr_t*);
extern void load_tr(); // loads task state register

void init_gdt() {
  gdt_ptr.limit = (uint16_t)(sizeof(gdt_entries)  - 1);
  gdt_ptr.base = (uint32_t)&gdt_entries;

  gdt_set_entry(0, 0, 0, 0, 0); // NULL entry
  gdt_set_entry(1, 0, 0xFFFFFFFF, 0x9A, 0xCF); // kernel code
  gdt_set_entry(2, 0, 0xFFFFFFFF, 0x92, 0xCF); // kernel data
  gdt_set_entry(3, 0, 0xFFFFFFFF, 0xFA, 0xCF); // user code
  gdt_set_entry(4, 0, 0xFFFFFFFF, 0xF2, 0xCF); // user data
  write_tss(5, 0x10, 0); // TSS

  load_gdtr(&gdt_ptr);
  load_tr();
}
