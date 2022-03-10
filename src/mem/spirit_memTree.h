#pragma once
#include <spirit_header.h>

// Custom memory managment binary trees. Each function is refactored to handle
// the different gap and ptr sorting methods needed to handle the preallocated stack
//
// 
// Kael Johnston Feb 29 2022

//
// MemGap
//

// new memgap thing to make life easy
#define new_memgap(size, ptr) (MemGap){\
    size, \
    ptr,  \
    NULL, \
    NULL  \
}

// memgap data
#define MemGapTree MemGap*

typedef struct t_MemGap {
    u64 size; // size of gap
    void *ptr; // location of gap
    struct t_MemGap *gthn, *lthn;
} MemGap;

MemGap newMemGapMap (void *start, u64 size);

MemGap getMemgapMax (MemGapTree gaps);

MemGap getMemgapMin (MemGapTree gaps);

// add a new gap to list
int memGapMap (const MemGapTree gaps, MemGap newGap);

// remove a gap from list
int memGapRemove (MemGapTree gaps, void *gapPtr);

// get the size of the gap at ptr
MemGap memGapGetSize (MemGapTree gaps, void *gapPtr);

// get a pointer to a gap with a minimum size
// gap->ptr == null is fail
MemGap memGapGetPtr (MemGapTree gaps, u64 minSize);

//
// MemPtr
//

// memchunk data
#define MemChunkTree MemChunk*

typedef struct t_MemChunk {
    void *ptr;
    u64 size;
    struct t_MemChunk *lthn, *gthn;
} MemChunk;

// get the size of a chunk at ptr
MemChunk getMemChunkSize (MemChunkTree chunks, void *ptr);

// map a new memchunk
int memChunkMap (MemChunkTree chunks, MemChunk newChunk);

// unmap a memchunk
int memChunkRemove (MemChunkTree chunks, void *ptr);
