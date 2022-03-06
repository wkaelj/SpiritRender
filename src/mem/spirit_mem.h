#pragma once
#include <spirit_header.h>

// Custom memory allocation and deallocation
// 
// 
// Kael Johnston

// possible memory types. each is stored in custom memory block
typedef enum e_MemTypes {
    SPIRIT_MEMTYPE_PERMANENT,
    SPIRIT_MEMTYPE_TEMP,
    SPIRIT_MEMTYPE_COUNT
} SpiritMemTypes;

// initialize spirit memory with base heap size x. Will allocate new memory in chunks of baseMemSize
SpiritResult spMemInit (u64 baseMemSize);

// terminal spirit memory. WILL FREE ALL POINTERS
SpiritResult spMemTerminate (SpiritMemory mem);

// allocate memory from heap
void *spMemAlloc (u64 size);

// free custom pointer
SpiritResult spMemFree (void *mem);

// moves all memory into clean blocks w/out gaps
SpiritResult spMemClean (SpiritMemTypes targetBlock);
