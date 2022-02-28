#pragma once
#include <spirit_header.h>

// Custom memory allocation and deallocation
// 
// 
// Kael Johnston

// initialize spirit memory with base heap size x. Will allocate new memory in chunks of baseMemSize
SpiritResult spMemInit (u64 baseMemSize);

// terminal spirit memory. WILL FREE ALL POINTERS
SpiritResult spMemTerminate (SpiritMemory mem);

// allocate memory from heap
void *spMespMemAlloc (u64 size);

// free custom pointer
SpiritResult spMemspMemFree (void *mem);