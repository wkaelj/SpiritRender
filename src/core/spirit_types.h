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

typedef enum e_SpiritResult {
    SPIRIT_SUCCESS = 0,  // total success
    SPIRIT_FAILURE = 1,  // certain failure
    SPIRIT_UNDEFINED = 2 // did not succeed, may not have failed
} SpiritResult;

//
// Structures
//

typedef struct t_SpiritResolution
{
    u64 w;
    u64 h;
} SpiritResolution;

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
} *SpiritDevice;

// actual window information is kept private because of 
// platform specific details
typedef struct t_SpiritWindow *SpiritWindow;

// render pass
typedef struct t_SpiritRenderPass {
    VkRenderPass renderPass;



} *SpiritRenderPass;

// swapchain
typedef struct t_SpiritSwapchain {

    // swapchain
    VkSwapchainKHR             swapchain;
    SpiritSwapchainSupportInfo supportInfo;


    VkExtent2D         extent;
    VkSurfaceFormatKHR surfaceFormat;
    VkPresentModeKHR   presentMode;

    // VkSwapchainCreateInfoKHR createInfo; // used to recreate

    // images
    VkImage     *images;
    u32          imageCount;
    VkImageView *imageViews;
    VkFormat imageFormat;

    VkImage *depthImages;
    VkImageView *depthImageViews;
    VkDeviceMemory *depthImageMemory;
    VkFormat depthFormat;

    // sync
    VkSemaphore *imageAvailableSemaphores;
    u32 imageAvailableSemaphoreCount;
    VkSemaphore *renderFinishedSemaphores;
    u32 renderFinishedSemaphoreCount;

    VkFence *inFlightFences;
    u32 inFlightFenceCount;
    VkFence *imagesInFlight;
    u32 imagesInFlightCount;

    VkFramebuffer *framebuffers;
    u32            framebufferCount;
    
    u32 currentFrame;

} *SpiritSwapchain;

// store a pointer to compiled shader code
typedef u32 *SpiritShaderCode;

// pipeline
typedef struct t_SpiritPipeline {

    VkPipeline    pipeline;
    VkPipelineLayout layout;    
    VkShaderModule *shaders;
    u32             shaderCount;
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

// spirit context
typedef struct t_SpiritContext
{
    SpiritWindow window;

    SpiritDevice     device;
    SpiritSwapchain  swapchain;
    SpiritRenderPass *renderPasses;
    u32               renderPassCount;
    SpiritPipeline  *pipelines;
    u32              pipelineCount;

    // command buffers
    VkCommandBuffer *commandBuffers;
    u32              commandBufferCount;

    SpiritResolution windowSize; // use for UI sizes, stored as screen units
} *SpiritContext;
