#pragma once
#include <spirit_header.h>

// Create a spirit render context
// Automatically initialize a rendering system
// with all the components

// include components


typedef struct t_SpiritContextCreateInfo
{

    // window
    SpiritResolution windowSize;
    const char *windowName;
    bool windowFullscreen;

    // device
    bool enableValidation; // should vulkan validation be initialized
    bool powerSaving; // should integrated GPU's be chosen

} SpiritContextCreateInfo;

struct t_SpiritContext
{
    SpiritWindow window;

    SpiritDevice     device;
    SpiritSwapchain  swapchain;

    // materials
    SpiritMaterial *materials; // TODO linkedlist
    u32 materialCount;

    // command buffers
    VkCommandBuffer *commandBuffers;
    size_t           commandBufferCount;
    u32              commandBufferIndex; // which command buffer to use to draw
    bool isRecording; // if the command buffers are currently recording

    SpiritResolution windowSize; // use for UI sizes, stored as screen units
    SpiritResolution screenResolution; // resolution in px, use for 
};

SpiritContext spCreateContext(SpiritContextCreateInfo *createInfo);

// perform basic tasks including:
//  - checking if context should remain open
//  - resizing the swapchain, if required
//  - selecting the next framebuffer
//  - beginning the command buffer for draw commands
SpiritContext spContextUpdateAndBegin(SpiritContext context);

SpiritResult spContextSubmitFrame(SpiritContext context);

// perform basic tasks including:
//  - checking if context should remain open
//  - resizing the swapchain, if required
//  - selecting the next framebuffer
//  - beginning the command buffer for draw commands
// and more complex rendering tasks such as
//  - submitting material commands to the swapchain
//  - issuing a draw call
SpiritResult spContextAddMaterial(
    SpiritContext context,
    SpiritMaterial material);

SpiritResult spContextRemoveMaterial(
    SpiritContext context,
    const char *materialName);

SpiritResult spDestroyContext(SpiritContext context);
