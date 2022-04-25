#pragma once
#include <spirit_header.h>

#include "spirit_window.h"

// Create and return rendering device
//
//
// Kael Johnston Feb 26 2022

//
// Types
//

// information used to create logical device
typedef struct t_SpiritDeviceCreateInfo {
    SpiritBool powerSaveMode; // prefer integrated GPU, and limit framerate
    SpiritBool enableValidation; // enable vulkan validation layers

    const char            *appName;
    u32                    appVersion;
    const char            *engineName;
    u32                   engineVersion;
    SpiritWindow           window;
    SpiritWindowExtensions windowExtensions; // available window extensions
    u32                    requiredValidationLayerCount;
    const char *const     *requiredValidationLayers;

    VkSurfaceKHR windowSurface; // will be overridden

    u32                requiredDeviceExtensionCount; // required GPU extension count
    const char *const *requiredDeviceExtensions; // required GPU extensions

} SpiritDeviceCreateInfo;


// create a spirit device
SpiritDevice spCreateDevice (SpiritDeviceCreateInfo *createInfo);

// destroy spirit device
SpiritResult spDestroyDevice (SpiritDevice device);