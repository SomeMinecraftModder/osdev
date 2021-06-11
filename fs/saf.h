#ifndef SAF_H
#define SAF_H

#include <stdint.h>

// Used to store offsets from start of archive
typedef uint64_t saf_offset_t;

typedef struct {
    uint64_t magic; // Must equal 0x766863726C706D73
    uint64_t len;   // Length of structure in bytes
    char name[256]; // Name of node
    uint64_t flags;
} __attribute__((packed)) saf_node_hdr_t;

typedef struct {
    saf_node_hdr_t hdr;
    uint64_t size;     // Size of file
    saf_offset_t addr; // Offset of file
} __attribute__((packed)) saf_node_file_t;

typedef struct {
    saf_node_hdr_t hdr;
    uint64_t num_children;   // No of children
    saf_offset_t children[]; // Array containing offsets of children
} __attribute__((packed)) saf_node_folder_t;

#endif
