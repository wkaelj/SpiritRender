#pragma once
// Header file stores global dependencies for project

// dmalloc
#ifdef DMALLOC
#include "dmalloc.h"
#endif

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
#include "utils/spirit_string.h"

// enable messeges upon success, not just failure
// object creation will log succesful creation as well as failure
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
#define log_verbose(messege, ...) ((void)0);
#endif

// tmp memory alocator
#define alloc(ptr) malloc(ptr);
#define dalloc(ptr) free(ptr);

// debug
#ifndef DEBUG
#define db_assert(statement, errmsg) \
    if (!(statement)) { \
        LOG_FATAL("Assertion '%s' failed: %s", #statement, #errmsg); \
        abort (); \
    }
#else
#define db_assert(statement, errmsg)
#endif
