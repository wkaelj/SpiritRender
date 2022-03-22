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

typedef unsigned char byte;

// predefined
typedef struct t_SpiritSwapchainSupportInfo {
    VkSurfaceCapabilitiesKHR capabilties;
    VkSurfaceFormatKHR *formats; // maybe need to fix pointers later
    u32 formatCount;
    VkPresentModeKHR *presentModes;
    u32 presentModeCount;
} SpiritSwapchainSupportInfo;

// memory
typedef struct t_SpiritMemory {
    void *memStart; // pointer to start of heap
    u64 memSize; // total heap size
} *SpiritMemory;

// window
typedef struct t_SpiritWindow *SpiritWindow; // wanna keep it private in case of platform specific windows
typedef struct t_SpiritWindowCreateInfo {
    int32_t w, h; // size of window in screen units
    char *title;
    bool fullscreen;
} SpiritWindowCreateInfo;

typedef struct t_SpiritWindowExtensions {
    u32 count;
    const char **names;
} SpiritWindowExtensions;

// rendering device
typedef struct t_SpiritDevice {
    VkDevice device;
    VkInstance instance;
    VkPhysicalDevice physicalDevice;
    VkDebugUtilsMessengerEXT debugMessenger;

    VkSurfaceKHR windowSurface;

    VkQueue graphicsQueue;
    VkQueue presentQueue;
    u32 queueCount;
    const u32 *queueIndices;

    const char *const *deviceExtensions;
    u32 deviceExtensionCount;

    SpiritBool powerSaveMode;
    SpiritBool validationEnabled;

    SpiritSwapchainSupportInfo swapchainDetails;
} *SpiritDevice;

typedef struct t_SpiritDeviceCreateInfo {
    SpiritBool powerSaveMode; // prefer integrated GPU, and limit framerate
    SpiritBool enableValidation; // enable vulkan validation layers

    char *appName;
    u32 appVersion;
    char *engineName;
    u32 engineVersion;
    SpiritWindow window;
    SpiritWindowExtensions windowExtensions; // available window extensions
    u32 requiredValidationLayerCount;
    const char *const *requiredValidationLayers;

    VkSurfaceKHR windowSurface; // will be overridden

    u32 requiredDeviceExtensionCount; // required GPU extension count
    const char *const *requiredDeviceExtensions; // required GPU extensions

} SpiritDeviceCreateInfo;

// swapchain
typedef struct t_SpiritSwapchainCreateInfo {

    SpiritBool selectedPresentMode;
    VkPresentModeKHR preferredPresentMode;
    SpiritBool selectedFormat;
    VkSurfaceFormatKHR preferedFormat;

    // dimensions of the window in pixels
    u32 windowWidthPx, windowHeightPx;

} SpiritSwapchainCreateInfo;

typedef struct t_SpiritSwapchain {
    VkSwapchainKHR swapchain;
    SpiritSwapchainSupportInfo supportInfo;

    VkExtent2D extent;
    VkSurfaceFormatKHR format;
    VkPresentModeKHR presentMode;

    VkSwapchainCreateInfoKHR createInfo; // used to recreate

    // images
    VkImage *images;
    u32 imageCount;
    VkImageView *imageViews;
} *SpiritSwapchain;

// pipeline
typedef struct t_SpiritPipeline {
    VkPipeline pipeline;
    VkRenderPass *renderPasses;
    uint32_t renderPassCount;
} *SpiritPipeline;

// render pass
typedef enum e_SpiritRenderPassType {
    SPIRIT_RENDER_PASS_TYPE_COLOUR,
    SPIRIT_RENDER_PASS_TYPE_SHADOW,
    SPIRIT_RENDER_PASS_TYPE_STENCIL,

    SPIRIT_RENDER_PASS_MAX
} SpiritRenderPassType;

// simplified render pass creation settings. can be converted to render pass create info using spRenderPassExpandSettings
typedef struct t_SpiritRenderPassSettings {

    SpiritRenderPassType type;
    f32 colourFactors[3]; // multiply colour values by this

} SpiritRenderPassSettings;

// complex render pass create info
typedef struct t_SpiritRenderPassCreateInfo {

} SpiritRenderPassCreateInfo;

// convert render pass settings into render pass create info
SpiritRenderPassCreateInfo spRenderPassExpandSettings (SpiritRenderPassSettings settings);

typedef struct t_SpiritRenderPass {
    VkRenderPass renderPass;
} *SpiritRenderPass;
