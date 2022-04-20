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
#define i8  int8_t
#define u8  uint8_t
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

typedef unsigned char byte;

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

//
// Vulkan Component Types
//

// logical device
typedef struct t_SpiritDevice {
    VkDevice                 device;
    VkInstance               instance;
    VkPhysicalDevice         physicalDevice;
    VkDebugUtilsMessengerEXT debugMessenger;

    VkSurfaceKHR windowSurface;

    VkQueue    graphicsQueue;
    VkQueue    presentQueue;
    u32        queueCount;
    const u32 *queueIndices;

    const char *const *deviceExtensions;
    u32                deviceExtensionCount;

    SpiritBool powerSaveMode;
    SpiritBool validationEnabled;

    SpiritSwapchainSupportInfo swapchainDetails;
} *SpiritDevice;


// render pass
typedef struct t_SpiritRenderPass {
    VkRenderPass renderPass;

} *SpiritRenderPass;

// swapchain
typedef struct t_SpiritSwapchain {
    VkSwapchainKHR             swapchain;
    SpiritSwapchainSupportInfo supportInfo;

    VkExtent2D         extent;
    VkSurfaceFormatKHR format;
    VkPresentModeKHR   presentMode;

    VkSwapchainCreateInfoKHR createInfo; // used to recreate

    // images
    VkImage     *images;
    u32          imageCount;
    VkImageView *imageViews;
} *SpiritSwapchain;

// pipeline
typedef struct t_SpiritPipeline {
    VkPipeline    pipeline;
    VkRenderPass *renderPasses;
    uint32_t      renderPassCount;
} *SpiritPipeline;