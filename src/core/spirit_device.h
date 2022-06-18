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
    bool powerSaveMode; // prefer integrated GPU, and limit framerate
    bool enableValidation; // enable vulkan validation layers

    const char            *appName;
    u32                    appVersion;
    const char            *engineName;
    u32                   engineVersion;
    SpiritWindow           window;
    SpiritWindowExtensions windowExtensions; // available window extensions
    u32                    requiredValidationLayerCount;
    const char *const     *requiredValidationLayers;

    VkSurfaceKHR windowSurface; // will be overridden   

    u32                requiredDeviceExtensionCount;
    const char *const *requiredDeviceExtensions;

} SpiritDeviceCreateInfo;

struct t_SpiritDevice
{
    VkDevice                 device;
    VkInstance               instance;
    VkPhysicalDevice         physicalDevice;
    VkDebugUtilsMessengerEXT debugMessenger;

    VkCommandPool commandPool;

    VkSurfaceKHR windowSurface;

    VkQueue    graphicsQueue;
    VkQueue    presentQueue;
    u32        queueCount;
    const u32 *queueIndices;

    const char *const *deviceExtensions;
    u32                deviceExtensionCount;

    bool powerSaveMode;
    bool validationEnabled;

    SpiritSwapchainSupportInfo swapchainDetails;
};

// create a spirit device
SpiritDevice spCreateDevice (SpiritDeviceCreateInfo *createInfo);

// find the format supported by the device
VkFormat spDeviceFindSupportedFormat(
    const SpiritDevice device,
    const VkFormat *candidates, 
    const u32 candidateCount, 
    const VkImageTiling tiling, 
    const VkFormatFeatureFlags features);

// find the memory type
u32 spDeviceFindMemoryType(
    const SpiritDevice device, 
    u32 typeFilter, 
    VkMemoryPropertyFlags properties);

// create a image using the specified vulkan device
SpiritResult spDeviceCreateImage(
    const SpiritDevice device,
    const VkImageCreateInfo *imageInfo,
    VkMemoryPropertyFlags memoryFlags,
    VkImage *image,
    VkDeviceMemory *imageMemory);

// destroy spirit device
SpiritResult spDestroyDevice (SpiritDevice device);
