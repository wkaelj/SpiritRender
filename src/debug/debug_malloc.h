#pragma once

#include <stddef.h>

#define malloc(size) \
    malloc_wrapper(size, __FILE__, __LINE__)
#define calloc(nmemb, size) \
    calloc_wrapper(nmemb, size, __FILE__, __LINE__)
#define free(ptr) free_wrapper(ptr)

void *malloc_wrapper(size_t s, const char *func, unsigned line);
void *calloc_wrapper(size_t count, size_t s, const char *func, unsigned line);

void free_wrapper(void *ptr);

void debug_malloc_dump_mem(void);
