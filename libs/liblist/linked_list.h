#ifndef LINKED_LIST
#define LINKED_LIST

#include <stdlib.h>

//
// An very easy linked list implementation
// I wanted to experiment with making C feel a little more object
// oriented without giving up any speed (or using C++).
//
// CURRENTLY USING TYPEOF(), SO ARRAYS MUST BE HANDLED BY USER (can be overloaded using size parameter)
//
// Kael Johnston, Feb 18th 2022

// define linkedlist type
typedef struct t_ListNode ListNode;

// define linkedlist handle
typedef struct t_LinkedList LinkedList;

// define list returns
typedef enum e_ListReturns {

    LIST_FAILURE = -1,
    LIST_SUCCESS = 0,
    LIST_INDEX_OVERFLOW = 2,
    LIST_INDEX_NULL = 3
} ListReturns;

// define function types
typedef void* (*t_get)(LinkedList *pList, size_t index, ListReturns *pState); // return index from list
typedef size_t (*t_find)(LinkedList *pList, void *pValue, ListReturns *pState); // Find value in linkedlist. Return NULL on failure. WARNING: Value gets dereferenced
typedef ListReturns (*t_append)(LinkedList *pList, void *pValue, size_t size); // push value to end of list
typedef ListReturns (*t_prepend)(LinkedList *pList, void *pValue); // push value to front of list
typedef ListReturns (*t_push)(LinkedList *pList, size_t index, void *pValue); // push value to index of list
typedef ListReturns (*t_pop)(LinkedList *pList, size_t index); // pop index out of list

struct t_LinkedList
{
    ListNode *head;
    ListNode *tail;

    size_t length;
    
    // function pointers
    t_get get;
    t_append append;
    t_prepend prepend;
    t_push push;
    t_pop pop;
};

// initialize new linkedlist (LinkedList x = newList ();)
LinkedList newList (void);

// delete a linked list
ListReturns deleteList (LinkedList *list);

// read a value from the list. Returns null on failure
static void *list_get (LinkedList *list, size_t index, ListReturns *state);
// check if list containes value, and return the index of that value (first occurance only) WARNING: Value gets dereferenced returns NULL on failure
static size_t list_find (LinkedList *list, void *value, ListReturns *state); 

// append a value to the end of list. if size is 0, will attempt to infer size using typeof
static ListReturns list_append (LinkedList *list, void *value, size_t size);

// push value to the beggining of list
static ListReturns list_prepend (LinkedList *list, void *value);

// push value to index of list (value becomes index of index)
static ListReturns list_push (LinkedList *list, size_t index, void *value);

// remove node from list at index
static ListReturns list_pop (LinkedList *list, size_t index);

#endif // header guard
