#pragma once
#include <spirit_header.h>
#include "spirit_renderpass.h"

// swapchain creation and recreation
//
//
// Kael Johnston Mar 10 2022

#include "spirit_device.h"

#define SPIRIT_SWAPCHAIN_MAX_FRAMES_IN_FLIGHT (2)

typedef struct t_SpiritSwapchainCreateInfo {

    // TODO vsync option
    bool         selectedPresentMode;
    VkPresentModeKHR   preferredPresentMode;
    bool         selectedFormat;
    VkSurfaceFormatKHR preferedFormat;

    // dimensions of the window in pixels
    u32 windowWidthPx, windowHeightPx;

} SpiritSwapchainCreateInfo;

struct t_SpiritSwapchain {

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

};

// create a new swapchain. Can optionaly pass a old swapchain, which will be recreated with new window resolution
SpiritSwapchain spCreateSwapchain (
    SpiritSwapchainCreateInfo createInfo,
    SpiritDevice              device, 
    SpiritSwapchain           optionalSwapchain);

SpiritResult spSwapchainSubmitCommandBuffer(
    SpiritDevice device,
    SpiritSwapchain swapchain,
    VkCommandBuffer buffer,
    u32 imageIndex);

// get the next image to render too
SpiritResult spSwapchainAquireNextImage(
    const SpiritDevice device,
    const SpiritSwapchain swapchain,
    u32 *imageIndex);

// add framebuffers for a specific render pass to a swapchain
// to use a different render pass, just call the function again
// the framebuffers will be automatically destroyed when the swapchain
// is destroyed
//  - device is the device used for everything
//  - swapchain is the swapchain to add framebuffers to
//  - renderPass is the render pass to use for the framebuffers
SpiritResult spSwapchainAddFramebuffers(
    const SpiritDevice device,
    SpiritSwapchain swapchain,
    const SpiritRenderPass renderPass);

// destroy a swapchain and free associated memory
SpiritResult spDestroySwapchain (
    SpiritSwapchain swapchain, 
    SpiritDevice device);
