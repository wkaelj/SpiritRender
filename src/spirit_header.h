#pragma once
// Header file stores global dependencies for project



// std

#define __STDC_WANT_LIB_EXT1__ 1
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include <assert.h>
#include <string.h>
#include <memory.h>
#include <time.h>
#include <sys/queue.h>

// dmalloc
#ifdef DMALLOC
#include "dmalloc.h"
#endif

// vulkan
#include <vulkan/vulkan.h>

// cglm
#define CGLM_FORCE_DEPTH_ZERO_TO_ONE
#include <cglm/cglm.h>

// custom 
#include "debug/messenger.h" // debug messenging functions
#include "core/spirit_types.h" // custom types
#include "utils/platform.h" // usefull stuff, like time and whatnot
#include "utils/spirit_string.h"

// enable messeges upon success, not just failure
// object creation will log succesful creation as well as failure
#define ENABLE_VERBOSE

// definitions
// create a new variable
#define new_var(typename) (typename*)malloc(sizeof(typename))
#define new_array(typename, count) (typename*)malloc(sizeof(typename) * count)

#define new_flex_array(structname, flexmembertype, elementcount) \
    ((structname*)malloc(\
            sizeof(structname) + \
            sizeof(flexmembertype) * elementcount))

// find the size of a STACK ALLOCATED array
#define array_length(array) = (sizeof(array)/sizeof(array[0]))

// functions to clamp value, work for all(numeric) types
#define min_value(x, y) ((x < y) ? x : y)
#define max_value(x, y) ((x > y) ? x : y)
#define clamp_value(value, min, max) (max_value(min, min_value(value, max)))

#ifdef ENABLE_VERBOSE
#define log_verbose(messege, ...) log_info(messege, ##__VA_ARGS__)
#else
#define log_verbose(messege, ...) ((void)0);
#endif

// tmp memory alocator
// will be replaced with calles to a custom memory allocator

// debug
#ifndef DEBUG
#define db_assert(statement, errmsg) \
    if (!(statement)) { \
        log_fatal("Assertion '%s' failed: %s", #statement, #errmsg); \
        abort (); \
    }
#else
#define db_assert(statement, errmsg) ((void)0)
#endif
