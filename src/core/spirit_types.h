#pragma once
#include <spirit_header.h>

// Variables for the spirit render.
// 
// 
// Kael Johnston Feb 26 2022

//
// Definitions
//

#define SPIRIT_NULL ((void*)0)

// types
#define i8 int8_t
#define u8 uint8_t
#define i16 int16_t
#define u16 uint16_t
#define i32 int32_t
#define u32 uint32_t
#define i64 int64_t
#define u64 uint64_t
#define f32 float
#define f64 double

//
// Enumerations
//

// function return values
typedef enum e_SpiritBool {
    SPIRIT_TRUE = 1,
    SPIRIT_FALSE = 0
} SpiritBool;

typedef enum e_SpiritResult {
    SPIRIT_SUCCESS = 0,
    SPIRIT_FAILURE = -1,
    SPIRIT_UNDEFINED = 1
} SpiritResult;

//
// Structures
//

// memory
typedef struct t_SpiritMemory {
    void *memStart; // pointer to start of heap
    u64 memSize; // total heap size
} *SpiritMemory;

// Custom pointer
typedef struct t_SpiritPointer {
    void *ptr;
    u64 size;
} SpiritPtr;

// window
typedef struct t_SpiritWindow *SpiritWindow; // wanna keep it private in case of platform specific windows
typedef struct t_SpiritWindowCreateInfo {
    int32_t w, h; // size of window in screen units
    char *title;
    bool fullscreen;
} SpiritWindowCreateInfo;

// rendering device
typedef struct t_SpiritDevice {
    VkDevice device;
    bool powerSaveMode;
} *SpiritDevice;

typedef struct t_SpiritDeviceCreateInfo {
    bool powerSaveMode;
} SpiritDeviceCreateInfo;

// swapchain
typedef struct t_SpiritSwapchain {
    VkSwapchainKHR swapchain;
} *SpiritSwapchain;

// pipeline
typedef struct t_SpiritPipeline {
    VkPipeline pipeline;
    VkRenderPass *renderPasses;
    uint32_t renderPassCount;
} *SpiritPipeline;
