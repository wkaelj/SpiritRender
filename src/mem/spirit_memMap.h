#pragma once
#include <spirit_header.h>

// Custom binary tree system to store allocated memory
// 
// 
// Kael Johnston

typedef struct t_MemNode {
    u64 ptr;
    u64 size;
    struct t_MemNode *gThen;
    struct t_MemNode *lThen;
} MemNode;

typedef struct t_MemTree {
    MemNode *head;
} MemTree;

// check if a list containes a certaion value
SpiritBool memTreeCheck (MemTree *tree, u64 value);

// add a pointer to the tree
SpiritResult memTreeAdd (MemTree *tree, u64 ptr, u64 size);

// get a pointer of at least size minSize
u64 memTreeGetPtr (MemTree *tree, u64 minSize);

// remove a pointer from the binary tree
SpiritResult memTreeFreePtr (MemTree *tree, u64 ptr, u64 size);