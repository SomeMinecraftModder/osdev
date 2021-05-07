#ifndef PAGING_H
#define PAGING_H

#include "isr.h"
#include <stdint.h>

typedef struct page {
    uint32_t present : 1;  // Page present in memory
    uint32_t rw : 1;       // Read-only if clear, readwrite if set
    uint32_t user : 1;     // Supervisor level only if clear
    uint32_t accessed : 1; // Has the page been accessed since last refresh?
    uint32_t dirty : 1;    // Has the page been written to since last refresh?
    uint32_t unused : 7;   // Amalgamation of unused and reserved bits
    uint32_t frame : 20;   // Frame address (shifted right 12 bits)
} page_t;

typedef struct page_table {
    page_t pages[1024];
} page_table_t;

typedef struct page_directory {
    page_table_t *tables[1024];
    uint32_t tablesPhysical[1024];
    uint32_t physicalAddr;
} page_directory_t;

page_t *get_page(uint32_t address, int make, page_directory_t *dir);
void alloc_frame(page_t *page, int is_kernel, int is_writeable);
void switch_page_directory(page_directory_t *new);
void identity_map_lfb(uint32_t location);
void page_fault(registers_t *regs);
void free_frame(page_t *page);
void init_paging();

#endif
