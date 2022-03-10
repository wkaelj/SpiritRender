#include "spirit_memTree.h"

// MemTree function implementations
//
//
// Kael Johnston Feb 29 2022

//
// Generics
//


//
// MemGap
//

MemGap getMemgapMax (MemGapTree gaps) {

    // failure
    if (gaps == NULL) return new_memgap(0, 0);

    // i can set the ptr bc it is just a copy. saves 4 byes of mem!
    while (gaps->gthn != NULL) gaps = gaps->gthn;

    // could probably just return *gaps, but this is stack
    return new_memgap(gaps->size, gaps->ptr);
}

MemGap getMemgapMin (MemGapTree gaps) {

    // failure
    if (gaps == NULL) return (MemGap){0, 0, NULL, NULL};

    while (gaps->lthn != NULL) gaps = gaps->lthn;

    return (MemGap){gaps->size, gaps->ptr, NULL, NULL};
}

int memGapMap (const MemGapTree gaps, MemGap newGap) {

    const MemGapTree tree = gaps;
    // what if some idiot has not set gaps yet
    if (tree == NULL) {
        gaps = &newGap;
        LOG_DEBUG("Mapped ptr '%p', *gaps->ptr");
        return SPIRIT_SUCCESS;
    }

    // what if gaps is null initalized
    if (tree->size == 0) {
        gaps = &newGap;
        return SPIRIT_SUCCESS;
    }
    // map pointer
    if (newGap.size > tree->size) {
        memGapMap(tree->gthn, newGap);
    } else if (newGap.size < tree->size){
        memGapMap(tree->lthn, newGap);
    } else return SPIRIT_FAILURE;



    return SPIRIT_SUCCESS;
}

int memGapRemove (MemGapTree gaps, void *gapPtr) {

    // get node to remove, store in gaps
    if (gaps == NULL) {
        LOG_ERROR("Pointer gap '%p' has not been allocated or tree is corrupt. tree head is NULL", gapPtr);
        return SPIRIT_FAILURE;
    }

    // remap surrounding nodes to maintain tree
    // use default memmap functions to avoid currupting tree
    MemGap ptr = memGapGetSize(gaps, gapPtr);
    // shoud technicaly remap all values beneath the ptr.lthn, bc its pointers will not be impacted
    // this does ensure that the tree will always be sorted correctly, without a risk of some values becoming hidden
    if (ptr.lthn != NULL) memGapMap(gaps, (MemGap){ ptr.lthn->size, ptr.lthn->ptr, ptr.lthn->gthn, ptr.lthn->lthn} );
    if (ptr.gthn != NULL) memGapMap(gaps, (MemGap){ ptr.gthn->size, ptr.gthn->ptr, ptr.gthn->gthn, ptr.gthn->lthn });
    

    return SPIRIT_SUCCESS;
}

MemGap memGapGetSize (MemGapTree gaps, void *gapPtr) {
    
    while (gaps->ptr != gapPtr) {
        if (gaps->gthn != NULL && gapPtr > gaps->ptr) gaps = gaps->gthn;
        else if (gaps->lthn != NULL && gapPtr < gaps->ptr) gaps = gaps->lthn;
        else {LOG_ERROR("Failed to locate gap at ptr '%p'", gapPtr); return (MemGap){};}
    }

    db_assert(gapPtr == gaps->ptr);

    return (MemGap){ gaps->size, gaps->ptr, NULL, NULL };
}

MemGap memGapGetPtr (MemGapTree gaps, u64 minSize) {

    // this should be an edge case
    if (gaps == NULL) {
        gaps = malloc (sizeof (MemGap));
        *gaps = (MemGap){ 0, NULL, NULL, NULL };
        LOG_WARNING("'gaps' ptr was unallocated");
    }

    i8 found = 0;
    // such logic
    // should find the smallest suitable pointer
    while (found == 0) {
        // move less if possible
        db_assert(gaps->size != 0);
        db_assert(gaps->ptr != 0);

        if (gaps->size > minSize && gaps->lthn != NULL && gaps->lthn->size > minSize) {
            gaps = gaps->lthn;
            db_assert(gaps->ptr != NULL);
        } else
        // if current is big enough choose that
        if (gaps->size > minSize) {
            db_assert(gaps->ptr != NULL);
            found = 1;
        } else
        // otherwise move greater
        if (gaps->gthn != NULL) {
            gaps = gaps->gthn;
            db_assert(gaps->ptr != NULL);
        }
        // if cannot go bigger, throw failure TODO should allocate new memory gap
        else {
            LOG_INFO("Allocating new memory, no suffucient gaps found");
            found = -1;
        }
    }

    return new_memgap(gaps->size, gaps->ptr);

}


//
// MemChunk
//


