#pragma once
#include <spirit_header.h>

// Spirit binary tree header. Currently for use in the memMap functions
//
//
// Kael Johnston Feb 28 2022

// tree null
#define TREE_NULL ((void*)0)

// node in binary tree
typedef struct t_TreeNode {
    void *value; // pointer to allocated chunk
    u64 size; // size of allocated chunk
    struct t_TreeNode *gthn, *lthn;
} TreeNode;

// binary tree type
typedef struct t_BinaryTree {
    TreeNode *trunk; // bottom of tree
} BinaryTree;

// possible tree failure states
typedef enum e_TreeResult {
    TREE_SUCCESS = 0,
    TREE_FAILURE = -1,
    TREE_MISSING_VALUE = 2
} TreeResult;

typedef enum e_TreeBool {
    TREE_FALSE = 0,
    TREE_TRUE = 1
} TreeBool;

// 
// Functions
//

// initalize a new binary tree
BinaryTree newTree (void);

// check if a value is contained in tree. 0 for false, and a value is the accocuated size
u64 treeCheckValue (BinaryTree *tree, void *ptr);

TreeResult treeAddValue (BinaryTree *tree, void *ptr, u64 size);

// can return missing value or success on a technical success
TreeResult treeRemoveValue (BinaryTree *tree, void *ptr);

// get largest value in tree
void *treeMaxValue (BinaryTree *tree);

// get smallest value in tree
void *treeMinValue (BinaryTree *tree);


