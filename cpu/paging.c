#include "paging.h"
#include "../drivers/screen.h"
#include "../kernel/panic.h"
#include "../libc/kheap.h"
#include <stdlib.h>
#include <string.h>

#ifdef __STRICT_ANSI__
    #define asm __asm__
#endif

// The kernel's page directory
page_directory_t *kernel_directory = 0;

// The current page directory;
page_directory_t *current_directory = 0;

// A bitset of frames - used or free
uint32_t *frames;
uint32_t nframes;

// The size of physical memory
#define mem_end_page ((mbi->mem_lower + mbi->mem_upper) / 1024) * 0x100000

// Defined in kheap.c
extern uint32_t placement_address;
extern heap_t *kheap;

// Macros used in the bitset algorithms
#define INDEX_FROM_BIT(a)  (a / (8 * 4))
#define OFFSET_FROM_BIT(a) (a % (8 * 4))

// Static function to set a bit in the frames bitset
static void set_frame(uint32_t frame_addr) {
    uint32_t frame = frame_addr / 0x1000;
    uint32_t idx = INDEX_FROM_BIT(frame);
    uint32_t off = OFFSET_FROM_BIT(frame);
    frames[idx] |= (0x1 << off);
}

// Static function to clear a bit in the frames bitset
static void clear_frame(uint32_t frame_addr) {
    uint32_t frame = frame_addr / 0x1000;
    uint32_t idx = INDEX_FROM_BIT(frame);
    uint32_t off = OFFSET_FROM_BIT(frame);
    frames[idx] &= ~(0x1 << off);
}

// Static function to find the first free frame
static uint32_t first_frame() {
    uint32_t i, j;
    for (i = 0; i < INDEX_FROM_BIT(nframes); i++) {
        if (frames[i] != 0xFFFFFFFF) {
            // At least one bit is free here
            for (j = 0; j < 32; j++) {
                uint32_t toTest = 0x1 << j;
                if (!(frames[i] & toTest)) {
                    return i * 4 * 8 + j;
                }
            }
        }
    }

    return -1;
}

// Function to allocate a frame
void alloc_frame(page_t *page, int is_kernel, int is_writeable) {
    if (page->frame != 0) {
        return;
    } else {
        uint32_t idx = first_frame();
        if (idx == (uint32_t)-1) {
            PANIC(__FILE__, __LINE__, "No free frames.");
        }

        set_frame(idx * 0x1000);
        page->present = 1;
        page->rw = (is_writeable) ? 1 : 0;
        page->user = (is_kernel) ? 0 : 1;
        page->frame = idx;
    }
}

// Function to deallocate a frame
void free_frame(page_t *page) {
    uint32_t frame;
    if (!(frame = page->frame)) {
        return;
    } else {
        clear_frame(frame);
        page->frame = 0x0;
    }
}

void init_paging() {
    nframes = mem_end_page / 0x1000;
    frames = (uint32_t *)malloc(INDEX_FROM_BIT(nframes));
    memset(frames, 0, INDEX_FROM_BIT(nframes));

    // Let's make a page directory
    kernel_directory = (page_directory_t *)kmalloc_a(sizeof(page_directory_t));
    memset(kernel_directory, 0, sizeof(page_directory_t));
    current_directory = kernel_directory;

    identity_map_lfb(mbi->framebuffer_addr);

    /* Map some pages in the kernel heap area.
     * Here we call get_page but not alloc_frame. This causes page_table_t's
     * to be created where necessary. We can't allocate frames yet because they
     * they need to be identity mapped first below, and yet we can't increase
     * placement_address between identity mapping and enabling the heap! */
    uint32_t i = 0;
    for (i = KHEAP_START; i < KHEAP_START + KHEAP_INITIAL_SIZE; i += 0x1000) {
        get_page(i, 1, kernel_directory);
    }

    /* We need to identity map (phys addr = virt addr) from
     * 0x0 to the end of used memory, so we can access this
     * transparently, as if paging wasn't enabled.
     * NOTE that we use a while loop here deliberately.
     * inside the loop body we actually change placement_address
     * by calling malloc(). A while loop causes this to be
     * computed on-the-fly rather than once at the start.
     * Allocate a lil' bit extra so the kernel heap can be
     * initialised properly */
    i = 0;
    while (i < placement_address + 0x1000) {
        // Kernel code is readable but not writeable from userspace
        alloc_frame(get_page(i, 1, kernel_directory), 0, 0);
        i += 0x1000;
    }

    // Now allocate those pages we mapped earlier
    for (i = KHEAP_START; i < KHEAP_START + KHEAP_INITIAL_SIZE; i += 0x1000) {
        alloc_frame(get_page(i, 1, kernel_directory), 0, 0);
    }

    // Before we enable paging, we must register our page fault handler
    register_interrupt_handler(14, page_fault);

    // Now, enable paging!
    switch_page_directory(kernel_directory);

    // Initialise the kernel heap
    kheap = create_heap(KHEAP_START, KHEAP_START + KHEAP_INITIAL_SIZE,
                        0xCFFFF000, 0, 0);
}

void identity_map_lfb(uint32_t location) {
    uint32_t j = location;
    while (j < location +
                 (mbi->framebuffer_pitch * (mbi->framebuffer_height + 16))) {
        // If frame is valid
        if (j + location +
              (mbi->framebuffer_pitch * (mbi->framebuffer_height + 16)) <
            mem_end_page) {
            set_frame(j); // Tell frame bitset this frame is in use
        }
        // Get page
        page_t *page = get_page(j, 1, kernel_directory);
        // Fill it
        page->present = 1;
        page->rw = 1;
        page->user = 1;
        page->frame = j / 0x1000;
        j += 0x1000;
    }
}

void switch_page_directory(page_directory_t *dir) {
    current_directory = dir;
    asm volatile("mov %0, %%cr3" ::"r"(&dir->tablesPhysical));
    uint32_t cr0;
    asm volatile("mov %%cr0, %0" : "=r"(cr0));
    cr0 |= 0x80000000; // Enable paging!
    asm volatile("mov %0, %%cr0" ::"r"(cr0));
}

page_t *get_page(uint32_t address, int make, page_directory_t *dir) {
    // Turn the address into an index
    address /= 0x1000;
    // Find the page table containing this address
    uint32_t table_idx = address / 1024;

    if (dir->tables[table_idx]) {
        return &dir->tables[table_idx]->pages[address % 1024];
    } else if (make) {
        uint32_t tmp;
        dir->tables[table_idx] =
          (page_table_t *)kmalloc_ap(sizeof(page_table_t), &tmp);
        memset(dir->tables[table_idx], 0, 0x1000);
        dir->tablesPhysical[table_idx] = tmp | 0x7; // PRESENT, RW, US
        return &dir->tables[table_idx]->pages[address % 1024];
    } else {
        return 0;
    }
}

void page_fault(registers_t *regs) {
    // A page fault has occurred
    // The faulting address is stored in the CR2 register
    uint32_t faulting_address;
    asm("mov %%cr2, %0" : "=r"(faulting_address));

    // The error code gives us details of what happened
    int present = !(regs->err_code & 0x1); // Page not present
    int rw = regs->err_code & 0x2;         // Write operation?
    int us = regs->err_code & 0x4;         // Processor was in user-mode?
    int reserved =
      regs->err_code & 0x8; // Overwritten CPU-reserved bits of page entry?

    char pg[256] = "";

    // Output an error message
    strcat(pg, "Page fault! ( ");
    if (present) {
        strcat(pg, "present ");
    }

    if (rw) {
        strcat(pg, "read-only ");
    }

    if (us) {
        strcat(pg, "user-mode ");
    }

    if (reserved) {
        strcat(pg, "reserved ");
    }
    strcat(pg, "), memory address: ");
    char c[256] = "";
    alt_hex_to_ascii_upper(faulting_address, c);
    strcat(pg, c);
    strcat(pg, ".");
    PANIC(__FILE__, __LINE__, pg);
}
