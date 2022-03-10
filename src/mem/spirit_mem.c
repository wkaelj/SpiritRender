#include "spirit_mem.h"

// Custom memory allocation implementation
// 
// 
// Kael Johnston Feb 27 2022

#include "spirit_memMap.h"

// portion of memory
typedef struct t_memchunk {
    void *ptr;
    u64 size;
    u32 id; // block id
} Memchunk;

// block of preallocated memory
typedef struct t_memblock {
    void *startptr;
    u64 size;
    Memmap map;
} Memblock;

//
// Variables
//

// array of memblocks, one for each type of memory
static Memblock memblocks[SPIRIT_MEMTYPE_COUNT];

//
// Public Functions
//

// initialize spirit memory with base heap size x. Will allocate new memory in chunks of baseMemSize
SpiritResult spMemInit (u64 baseMemSize) {


    return SPIRIT_SUCCESS;
}

// terminal spirit memory. WILL FREE ALL POINTERS
SpiritResult spMemTerminate (SpiritMemory mem) {

    free (mem);
    return SPIRIT_SUCCESS;
}

// allocate memory from heap
void *spMemAlloc (u64 size) {

    return malloc (size);
}

// free custom pointer
SpiritResult spMemFree (void *mem) {

    if (mem != NULL) free (mem);
    return SPIRIT_SUCCESS;
}
