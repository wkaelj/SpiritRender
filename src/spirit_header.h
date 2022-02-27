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
#include "debug/messenger.h"
#include "core/spirit_types.h"

// definitions
#define new_var(typename) (typename)malloc(sizeof(typename))
#define new_array(typename, count) (typename*)calloc(count, sizeof(typename))

#endif