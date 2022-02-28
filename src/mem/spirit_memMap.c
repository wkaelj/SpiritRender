#include "spirit_memMap.h"

// memory binary tree implementation.
// It stores all memory in a binary tree referencing the pointer, with a
// size.
//
//
// Kael Johnston Feb 28 2022

// check if a list containes a certaion value
SpiritBool memTreeCheck (MemTree *tree, u64 value) {
    
    MemNode *node = tree->head; // used to buffer pointer info

    while (node->ptr != value) {

        // if greater value is greater then value, go to small value
        if (node -> gThen != SPIRIT_NULL && node->gThen->ptr > value) {
            node = node->lThen;
        } else if (node->lThen != SPIRIT_NULL && node-> lThen->ptr < value){
            node = node->gThen; // otherwise set node to be greater
        } else {
            return SPIRIT_FALSE;
        }
    }

    return SPIRIT_TRUE;
}

// add a pointer to the tree
SpiritResult memTreeAdd (MemTree *tree, u64 ptr, u64 size) {
    
    // look at my cool new initalizer
    MemNode newNode = {ptr, size, SPIRIT_NULL, SPIRIT_NULL};
    MemNode *x = tree->head;

    // work loop
    while (x->gThen != &newNode && x->lThen != &newNode) {
        
        // if ptr is greater then x->ptr go greater then
        if (x->ptr < ptr) {
            if (x->gThen == SPIRIT_NULL) x->gThen = &newNode;
            else x = x->gThen;
        } else if (x->ptr > ptr) {
            if (x->lThen == SPIRIT_NULL) x->lThen = &newNode;
            else x = x->lThen;
        } else {
            LOG_FATAL("Failed to allocate memory");
            return SPIRIT_FAILURE;
        }
    }

    return SPIRIT_SUCCESS;
}

// find a pointer with a minimum given size
u64 memTreeGetPtr (MemTree *tree, u64 minSize) {
    
}
