#pragma once
#include <spirit_header.h>

// Variables for the spirit render.
// 
// 
// Kael Johnston Feb 26 2022

//
// Definitions
//

#undef NULL
#define NULL ((void*)0)

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
    SPIRIT_SUCCESS = 0,  // total success
    SPIRIT_FAILURE = 1,  // certain failure
    SPIRIT_UNDEFINED = 2 // did not succeed, may not have failed
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

    // VkSwapchainCreateInfoKHR createInfo; // used to recreate

    // images
    VkImage     *images;
    u32          imageCount;
    VkImageView *imageViews;
} *SpiritSwapchain;

// store a pointer to compiled shader code
typedef u32 *SpiritShaderCode;

// pipeline
typedef struct t_SpiritPipeline {
    VkPipeline    pipeline;

    // render passes
    VkRenderPass *renderPasses;
    uint32_t      renderPassCount;

    VkShaderModule   *shaders;
    SpiritShaderCode *shaderCodes;
    u32               shaderCount;
} *SpiritPipeline;

// shaders
// store wether a shader is frag or vert shader
typedef enum t_SpiritShaderType {
    SPIRIT_SHADER_TYPE_AUTO_DETECT, // not recommended
    SPIRIT_SHADER_TYPE_VERTEX,
    SPIRIT_SHADER_TYPE_FRAGMENT,
    SPIRIT_SHADER_TYPE_COMPUTE,

    SPIRIT_SHADER_TYPE_MAX
} SpiritShaderType;

// store a vulkan (.spv) shader
typedef struct t_SpiritShader {
    SpiritShaderType type;
    const char      *path;
    SpiritShaderCode shader;
    u64              shaderSize;
} SpiritShader;