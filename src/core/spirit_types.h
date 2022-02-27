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

//
// Enumerations
//

// function return values
typedef enum e_SpiritReturns {
    SPIRIT_SUCCESS = 0,
    SPIRIT_FAILURE = -1,
    SPIRIT_TRUE = 1,
    SPIRIT_FALSE = 0
} SpiritReturns;

//
// Structures
//

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
} *SpiritDevice;

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