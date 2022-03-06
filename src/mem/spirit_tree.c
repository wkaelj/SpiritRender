#include "spirit_tree.h"

// Binary tree implementation
//
//
// Kael Johnston Feb 28 2022

// get a treeNode with a value
static TreeBool getNode (BinaryTree *tree, void *value, TreeNode *out);

BinaryTree newTree (void) {
    // it only uses stack BOOIIIS
    BinaryTree out = {TREE_NULL};
    return out;
}

u64 treeCheckValue (BinaryTree *tree, void *ptr) {
    
    TreeNode node;
    if (getNode(tree, ptr, &node) != TREE_TRUE) return 0;
    else return node.size;
}


// add value to tree
TreeResult treeAddValue (BinaryTree *tree, void *ptr, u64 size) {

    TreeNode newNode = {ptr, size, TREE_NULL, TREE_NULL};
    TreeNode *node = tree->trunk;
    // if no values in tree
    if (node == TREE_NULL) {
        node = &newNode;
        return TREE_SUCCESS;
    }

    // find available node. getnode does not work bc there
    // is no node to find yet
    while (node->gthn != &newNode && node->lthn != &newNode) {
        
        // greater than
        if (ptr > node->value) {
            if (node->gthn != TREE_NULL) node = node->gthn; // is null
            else { node->gthn = &newNode; return TREE_SUCCESS; } // is not null
        } else 
        // less than
        if (ptr < node->value) {
            if (node->lthn != TREE_NULL) node = node->gthn; // is not null
            else { node->lthn = &newNode; return TREE_SUCCESS; } // is null
        }
    }
    
    // it should never get here
    LOG_FATAL("Never reached and of binary tree");
    return TREE_FAILURE;
}

// getNode implementation
static TreeBool getNode (BinaryTree *tree, void *ptr, TreeNode *out) {
    
    TreeNode *x = tree->trunk;
    while (x->value != ptr) {
        if (x->value < ptr && x->lthn != TREE_NULL) x = x->lthn;
        else if (x->value > ptr && x->gthn != TREE_NULL) x = x->gthn;
        else return TREE_FALSE;
    }

    *out = *x; // hopfully no segfaults
    return TREE_TRUE;
}

// tree max and min value implementation
void *treeMaxValue (BinaryTree *tree) {
    
    TreeNode *node = tree->trunk;
    if (node == TREE_NULL) return TREE_NULL;

    // traverse to end of tree
    while (node->gthn != TREE_NULL) {
        node = node->gthn;
    }

    return node->value;
}

void *treeMinValue (BinaryTree *tree) {
    
    TreeNode *node = tree->trunk;
    if (node == TREE_NULL) return TREE_NULL;

    // traverse to least value
    while (node->lthn != TREE_NULL) {
        node = node->lthn;
    }

    return node->value;
}
