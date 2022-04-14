#pragma once
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
#include "utils/spirit_string.h"

// enable sum stuff
#define ENABLE_VERBOSE

// definitions
#define new_var(typename) malloc(sizeof(typename))
#define new_array(typename, count) malloc(sizeof(typename) * count)
#define array_length(array) = (sizeof(array)/sizeof(array[0]))

#define min_value(x, y) ((x < y) ? x : y)
#define max_value(x, y) ((x > y) ? x : y)
#define clamp_value(value, min, max) (max_value(min, min_value(value, max)))

#ifdef ENABLE_VERBOSE
#define log_verbose(messege, ...) LOG_INFO(messege, ##__VA_ARGS__)
#else
#define log_verbose(messege, ...) ;
#endif

// tmp memory alocator
#define alloc(ptr) malloc(ptr);
#define dalloc(ptr) free(ptr);

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
