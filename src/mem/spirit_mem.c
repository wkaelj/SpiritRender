#include "spirit_mem.h"

// Custom memory allocation implementation
// 
// 
// Kael Johnston Feb 27 2022

#include "spirit_memMap.h"
typedef struct t_memblock {
    void *startptr;
    u64 size;

    MemTree memMap;

} memblock;

//
// Variables
//

static memblock *memblocks;
static u64 memblockcount;

//
// Public Functions
//

// initialize spirit memory with base heap size x. Will allocate new memory in chunks of baseMemSize
SpiritResult spMemInit (u64 baseMemSize) {

    memblocks[0].startptr = malloc (baseMemSize); // yes I know

    return SPIRIT_SUCCESS;
}

// terminal spirit memory. WILL FREE ALL POINTERS
SpiritResult spMemTerminate (SpiritMemory mem) {

    free (mem);
    return SPIRIT_SUCCESS;
}

// allocate memory from heap
void *spMespMemAlloc (u64 size) {

    void *ptr;
    ptr = spMemAlloc (size);
    return ptr;
}

// free custom pointer
SpiritResult spMemspMemFree (void *mem) {

    return SPIRIT_SUCCESS;
}