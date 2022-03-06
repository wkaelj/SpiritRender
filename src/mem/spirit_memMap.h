#pragma once
#include <spirit_header.h>

#include "spirit_mem.h"
#include "spirit_memTree.h"

// Manage the accesable pointers of a memblock
//
// 
// Kael Johnston Feb 28 2022

// memory map, used to store the memory indices of a memblock
// must map both heads as NULL on creation
typedef struct t_memmap {
    MemGap *gaps;
    MemChunk *chunks;
} Memmap;

// map a chunk of memory. only guaranteed to be minsize
// also guaranteed to be a unique pointer
void *spMemMap (u64 minSize);

// unmap memory
SpiritResult spMemUnmap (void *ptr, u64 size);

// remap memory to new memmap without
Memmap spMemRemap (Memmap mem, u64 newSize);
