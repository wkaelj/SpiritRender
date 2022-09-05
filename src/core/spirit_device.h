#pragma once
#include <spirit_header.h>
#include <vulkan/vulkan_core.h>

#include "core/spirit_types.h"
#include "spirit_window.h"

// Create and return rendering device
//
//
// Kael Johnston Feb 26 2022

//
// Types
//

// device information used by the swapchain code
<<<<<<< HEAD
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
=======
// it is declared here so that neither the device or the swapchain need to
// include the other
typedef struct t_SpiritSwapchainSupportInfo
{
    VkSurfaceCapabilitiesKHR capabilties;
    VkSurfaceFormatKHR *formats; // maybe need to fix pointers later
    u32 formatCount;
    VkPresentModeKHR *presentModes;
    u32 presentModeCount;
} SpiritSwapchainSupportInfo;
// information used to create logical device
typedef struct t_SpiritDeviceCreateInfo
{
    bool powerSaveMode;    // prefer integrated GPU, and limit framerate
    bool enableValidation; // enable vulkan validation layers

    const char *appName;
    u32 appVersion;
    const char *engineName;
    u32 engineVersion;
    SpiritWindow window;
>>>>>>> devel
    SpiritWindowExtensions windowExtensions; // available window extensions
    u32 requiredValidationLayerCount;
    const char *const *requiredValidationLayers;

    VkSurfaceKHR windowSurface; // will be overridden

    u32 requiredDeviceExtensionCount;
    const char *const *requiredDeviceExtensions;

} SpiritDeviceCreateInfo;

struct t_SpiritDevice
{
<<<<<<< HEAD
    VkDevice                 device;
    VkInstance               instance;
    VkPhysicalDevice         physicalDevice;
=======
    VkDevice device;
    VkInstance instance;
    VkPhysicalDevice physicalDevice;
>>>>>>> devel
    VkDebugUtilsMessengerEXT debugMessenger;

    VkCommandPool commandPool;

    VkSurfaceKHR windowSurface;

<<<<<<< HEAD
    VkQueue    graphicsQueue;
    VkQueue    presentQueue;
    u32        queueCount;
    const u32 *queueIndices;

    const char **deviceExtensions;
    u32                deviceExtensionCount;
=======
    VkQueue graphicsQueue;
    VkQueue presentQueue;
    u32 queueCount;
    const u32 *queueIndices;

    const char **deviceExtensions;
    u32 deviceExtensionCount;
>>>>>>> devel

    bool powerSaveMode;
    bool validationEnabled;

    SpiritSwapchainSupportInfo swapchainDetails;
};

// create a spirit device
SpiritDevice spCreateDevice(SpiritDeviceCreateInfo *createInfo);

// find the format supported by the device
VkFormat spDeviceFindSupportedFormat(
    const SpiritDevice device,
    const VkFormat *candidates,
    const u32 candidateCount,
    const VkImageTiling tiling,
    const VkFormatFeatureFlags features) SPIRIT_NONULL(2);

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
    const u32 typeFilter,
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
 * @brief Allocate gpu memory
 *
 * @param device a valid SpiritDevice
 * @param size the amount of memory (bytes)
 * @param memoryType the type of memory (spDeviceFindMemoryType)
 * @param memory the VkDeviceMemory object to use for the memory
 * @return SpiritResult
 */
SpiritResult spDeviceAllocateMemory(
    const SpiritDevice device,
    const size_t size,
    const u32 memoryType,
    VkDeviceMemory *memory) SPIRIT_NONULL(4);

/**
 * @brief Free memory allocated by spDeviceAllocateMemory
 *
 * @param device the device used to allocate the memory
 * @param memory a valid VkDeviceMemory object
 */
SPIRIT_INLINE void
spDeviceFreeMemory(const SpiritDevice device, VkDeviceMemory memory)
{
    vkFreeMemory(device->device, memory, ALLOCATION_CALLBACK);
}

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
    SpiritDevice device,
    VkDeviceSize size,
    VkBufferUsageFlags usage,
    VkMemoryPropertyFlags properties,
    VkBuffer *buffer,
    VkDeviceMemory *bufferMemory) SPIRIT_NONULL(5, 6);

/**
 * @brief
 *
 *
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
    VkDeviceMemory *imageMemory) SPIRIT_NONULL(2, 4, 5) SPIRIT_DEPRECATED;

SPIRIT_INLINE void spDeviceWaitIdle(const SpiritDevice device)
{
    vkDeviceWaitIdle(device->device);
}

/**
 * @brief Create an image view for an existing image
 *
 * @param device
 * @param imageFormat the image format, can be found using
 spDeviceFindSupportedFormat
 * @param image the image to create an imageView for
 * @return VkImageView will be null on failure
 */
VkImageView spDeviceCreateImageView(
    const SpiritDevice device,
    const VkFormat imageFormat,
    const VkImage image,
    VkImageAspectFlags);

<<<<<<< HEAD
// find the format supported by the device
VkFormat spDeviceFindSupportedFormat(
    const SpiritDevice device,
    const VkFormat *candidates,
    const u32 candidateCount,
    const VkImageTiling tiling,
    const VkFormatFeatureFlags features) SPIRIT_NONULL(2);

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
    const u32 typeFilter,
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
 * @brief Allocate gpu memory
 *
 * @param device a valid SpiritDevice
 * @param size the amount of memory (bytes)
 * @param memoryType the type of memory (spDeviceFindMemoryType)
 * @param memory the VkDeviceMemory object to use for the memory
 * @return SpiritResult
 */
SpiritResult spDeviceAllocateMemory(
    const SpiritDevice device,
    const size_t size,
    const u32 memoryType,
    VkDeviceMemory *memory) SPIRIT_NONULL(4);

/**
 * @brief Free memory allocated by spDeviceAllocateMemory
 *
 * @param device the device used to allocate the memory
 * @param memory a valid VkDeviceMemory object
 */
SPIRIT_INLINE void spDeviceFreeMemory(
    const SpiritDevice device,
    VkDeviceMemory memory)
{
    vkFreeMemory(device->device, memory, ALLOCATION_CALLBACK);
}

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
    VkDeviceMemory       *bufferMemory) SPIRIT_NONULL(5, 6);

/**
 * @brief (DEPRACATED) Create an image on the associated device
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
    VkDeviceMemory *imageMemory) SPIRIT_NONULL(2, 4, 5) SPIRIT_DEPRECATED;

SPIRIT_INLINE void spDeviceWaitIdle(const SpiritDevice device)
{
    vkDeviceWaitIdle(device->device);
}

/**
 * @brief Create an image view for an existing image
 *
 * @param device
 * @param imageFormat the image format, can be found using
 spDeviceFindSupportedFormat
 * @param image the image to create an imageView for
 * @return VkImageView will be null on failure
 */
VkImageView spDeviceCreateImageView(
    const SpiritDevice device,
    const VkFormat imageFormat,
    const VkImage image,
    VkImageAspectFlags);

=======
>>>>>>> devel
/**
 * @brief Destroy a vulkan device
 *
 * @param device
 * @return SpiritResult
 */
SpiritResult spDestroyDevice(SpiritDevice device);
