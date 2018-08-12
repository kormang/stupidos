#ifndef PAGING_H
#define PAGING_H
#include "common.h"

typedef void* virt_addr_t;

typedef union {
  virt_addr_t addr;
  struct {
    uint32_t offset   :12;
    uint32_t pt_index :10;
    uint32_t pd_index :10;
  };
} virt_addr_struct_t;

typedef void* phys_addr_t;

typedef union {
  struct {
    uint32_t present :1; // P - page present in memory
    uint32_t rw :1; // R - 0 - read-only, 1 - read-write
    uint32_t user :1; // U - if clear - supervisor level only
    uint32_t write_through :1; // W - if set, write-through caching is enabled
    uint32_t cache_disabled :1; // C - disable cache
    uint32_t accessed :1; // A - accessed since last refresh
    uint32_t dirty :1; // page is written to
    uint32_t zero :1;
    uint32_t global :1; // G - prevents TLB from updating the address when CR3 is reset
    uint32_t unused :3; // available for arbitrary use
    uint32_t frame :20; // frame address
  };
  uint32_t flags :12;
  uint32_t entry;
}__attribute__((packed)) page_t;

typedef struct {
  page_t pages[1024];
}page_table_t;

typedef union {
  struct {
    uint32_t present :1; // P - page present in memory
    uint32_t rw :1; // R - 0 - read-only, 1 - read-write
    uint32_t user :1; // U - if clear - supervisor level only
    uint32_t write_through :1; // W - if set, write-through caching is enabled
    uint32_t cache_disabled :1; // D - disable cache
    uint32_t accessed :1; // A - accessed since last refresh
    uint32_t zero :1;
    uint32_t page_size :1; // S - 0 for 4kB
    uint32_t ignored :1; // G - ignored
    uint32_t unused :3; // available for arbitrary use
    uint32_t page_table_addr :20; // address of page table
  };
  uint32_t flags :12;
  uint32_t entry;
}__attribute__((packed)) pd_entry_t;

typedef struct {
  pd_entry_t page_tables[1024];
  page_table_t* virt_tables[1024]; // virtual addresses of tables
}page_directory_t;

void init_paging();

void switch_page_directory(page_directory_t* pd);

// if make_pt then create page table if necessary
page_t* get_page(uint32_t address, int make_pt, page_directory_t* pd);

phys_addr_t virt_to_phys(virt_addr_t virtualaddr);
void map_page (phys_addr_t physaddr, virt_addr_t virtaddr, uint32_t flags);

#endif
