#include <assert.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/queue.h>

typedef struct t_AllocationListNode
{
    void *ptr;
    size_t size;
    // backtrace
    const char *func;
    unsigned line;

    LIST_ENTRY(t_AllocationListNode) data;
} AllocationListNode;

struct
{
    LIST_HEAD(t_AllocationListHead, t_AllocationListNode) list;
    unsigned allocationCount;
} debugMallocData = {.allocationCount = 0, .list.lh_first = NULL};

__attribute__((destructor)) void terminate_debug_malloc(void)
{
    if (!LIST_EMPTY(&debugMallocData.list))
    {
        printf("Leaked Memory: \n");
    }

    size_t totalLeaked                    = 0;
    struct t_AllocationListNode *np       = NULL;
    struct t_AllocationListNode *lastNode = NULL;
    LIST_FOREACH(np, &debugMallocData.list, data)
    {
        totalLeaked += np->size;
        printf("\t%lu bytes from '%s:%u\n'", np->size, np->func, np->line);
        LIST_REMOVE(np, data);
        if (lastNode)
            free(lastNode);
        lastNode = np;
    }

    if (totalLeaked)
        printf("Total leaked memory %lu\n", totalLeaked);

    if (lastNode)
        free(lastNode);
    assert(LIST_EMPTY(&debugMallocData.list));
}

__attribute_malloc__ void *
malloc_wrapper(size_t s, const char *func, unsigned line)
{
    // allocate extra memory so there is room for the node to be stored with it
    // this makes the free function more efficient 7FFFF7FFE2D0
    AllocationListNode *newnode = malloc(sizeof(AllocationListNode) + s);

    newnode->func = func;
    newnode->line = line;
    newnode->size = s;
    newnode->ptr  = (void*)((unsigned long) newnode + sizeof(AllocationListNode));
    assert(
        (unsigned long)newnode + sizeof(AllocationListNode) ==
        (unsigned long)newnode->ptr);
    LIST_INSERT_HEAD(&debugMallocData.list, newnode, data);

    ++debugMallocData.allocationCount;

    // the memory offset from newnode
    return newnode->ptr;
}

__attribute_malloc__ void *
calloc_wrapper(size_t n, size_t s, const char *func, unsigned line)
{
    void *p = malloc_wrapper(n * s, func, line);
    memset(p, 0, n * s);
    return p;
}

void free_wrapper(void *ptr)
{
    // catch some very rare double frees
    if (debugMallocData.allocationCount == 0)
        printf("Invalid free of pointer %p", ptr - sizeof(AllocationListNode));

    // find the node allocated with the memory
    AllocationListNode *np = ptr - sizeof(AllocationListNode);
    LIST_REMOVE(np, data);

    free(np);

    --debugMallocData.allocationCount;
}

void debug_malloc_dump_mem(void)
{
    if (!LIST_EMPTY(&debugMallocData.list))
        printf("Currently Used Memory:\n");

    AllocationListNode *np;
    size_t total = 0;
    LIST_FOREACH(np, &debugMallocData.list, data)
    {
        printf(
            "\t%lu bytes in use from %s:%i, as %p\n",
            np->size,
            np->func,
            np->line,
            np->ptr);
        total += np->size;
    }

    if (total)
        printf("Total memory in use: %lu\n", total);
}