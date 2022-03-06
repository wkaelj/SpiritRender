#ifndef SPIRIT_HEADER
#define SPIRIT_HEADER

// Header file stores global dependencies for project

// std
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include <assert.h>

// vulkan
#include <vulkan/vulkan.h>

// custom 
#include "debug/messenger.h" // debug messenging functions
#include "core/spirit_types.h" // custom types
#include "mem/spirit_mem.h" // heap memory allocator

// definitions
#define new_var(typename) spMemAlloc(sizeof(typename))
#define new_array(typename, count) spMemAlloc(sizeof(typename) * count)
#define array_length(array) = (sizeof(array)/sizeof(array[0]))

// debug
#ifndef DEBUG
#define db_assert(statement) \
    if (!(statement)) { \
        LOG_FATAL("Assertion '%s' failed", #statement); \
        exit (1); \
    }
#else
#define db_assert(statement)
#endif

#endif
