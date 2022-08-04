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

// device information used by the swapchain code
// it is declared here so that neither the device or the swapchain need to include
// the other
typedef struct t_SpiritSwapchainSupportInfo {
    VkSurfaceCapabilitiesKHR capabilties;
    VkSurfaceFormatKHR      *formats; // maybe need to fix pointers later
    u32                      formatCount;
    VkPresentModeKHR        *presentModes;
    u32                      presentModeCount;
} SpiritSwapchainSupportInfo;
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

/**
 * @brief Find the available memory type from a list of types
 * 
 * @param device 
 * @param typeFilter 
 * @param properties 
 * @return u32 
 */
u32 spDeviceFindMemoryType(
    const SpiritDevice device, 
    u32 typeFilter, 
    VkMemoryPropertyFlags properties);

/**
 * @brief Update the stored swapchain support information of a device.
 * This will update the image size contraints, which will make it possible to
 * resize the window.
 * 
 * @param device 
 * @return SpiritResult 
 */
SpiritResult spDeviceUpdateSwapchainSupport(SpiritDevice device);

/**
 * @brief Create a buffer on the associated device
 * 
 * @param device the device with the buffer
 * @param size the size of the buffer
 * @param usage how to buffer will be used
 * @param properties 
 * @param buffer the output buffer
 * @param bufferMemory a pointer to the memory for that buffer
 * @return SpiritResult 
 */
SpiritResult spDeviceCreateBuffer(
    SpiritDevice          device,
    VkDeviceSize          size,
    VkBufferUsageFlags    usage,
    VkMemoryPropertyFlags properties,
    VkBuffer             *buffer,
    VkDeviceMemory       *bufferMemory);

/**
 * @brief Create an image on the associated device
 * 
 * @param device 
 * @param imageInfo 
 * @param memoryFlags 
 * @param image 
 * @param imageMemory 
 * @return SpiritResult 
 */
SpiritResult spDeviceCreateImage(
    const SpiritDevice device,
    const VkImageCreateInfo *imageInfo,
    VkMemoryPropertyFlags memoryFlags,
    VkImage *image,
    VkDeviceMemory *imageMemory);

/**
 * @brief Destroy a vulkan device
 * 
 * @param device 
 * @return SpiritResult 
 */
SpiritResult spDestroyDevice (SpiritDevice device);
