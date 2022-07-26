#pragma once
#include <spirit_header.h>
#include "spirit_renderpass.h"
#include "spirit_device.h"

/**
 * Manage and wrap a vulkan swapchain.
 * 
 * @author Kael Johnston
 * @date June 23 2022
 */


/**
 * Used to configure an SpiritSwapchain.
 * 
 * @author Kael Johnston
 */
typedef struct t_SpiritSwapchainCreateInfo
{

    // TODO vsync option
    bool               selectedPresentMode;
    VkPresentModeKHR   preferredPresentMode;
    bool               selectedFormat;
    VkSurfaceFormatKHR preferedFormat;

    // dimensions of the window in pixels
    u32 windowWidthPx, windowHeightPx;

} SpiritSwapchainCreateInfo;

/**
 * A object wrapping the Vulkan VkSwapchainKHR object. It is used to transfer
 * images that have been rendered to the screen.
 * 
 * It can be created using spCreateSwapchain(), and destroyed using spCreateSwapchain.
 * It depends on a SpiritDevice, and is used by the renderpasses and the context.
 * 
 * all sync objects use maxFramesInFlight to store their sizes.
 * 
 * @author Kael Johnston
 */
struct t_SpiritSwapchain
{

    // swapchain
    VkSwapchainKHR             swapchain;
    SpiritSwapchainSupportInfo supportInfo;


    VkExtent2D         extent; // size of the images
    VkSurfaceFormatKHR surfaceFormat;
    VkPresentModeKHR   presentMode;

    // VkSwapchainCreateInfoKHR createInfo; // used to recreate

    // images
    VkImage     *images;
    u32          imageCount;
    VkImageView *imageViews;
    VkFormat     imageFormat;

    VkImage        *depthImages;
    VkImageView    *depthImageViews;
    VkDeviceMemory *depthImageMemory;
    VkFormat        depthFormat;

    // sync
    u32 maxFramesInFlight; // the size of all sync objects
    VkSemaphore *imageAvailableSemaphores;
    VkSemaphore *renderFinishedSemaphores;

    VkFence *fences;
    u32 currentFence; // the fence currently rendering

    // framebuffers
    VkFramebuffer *framebuffers;
    u32            framebufferCount;
    
    u32 currentFrame;

};

/**
 * Create a new vulkan swapchain. A swapchain should be used by the context,
 * and contained by the context.
 * 
 * The swapchain stores images which can be displayed to the screen.
 * 
 * @param createInfo must be a SpiritSwapchainCreateInfo struct, which will be
 * used to configure the swapchain.
 * @param device a valid SpiritDevice, created using spCreateDevice or automatically
 * using a context.
 * @param optionalSwapchain is an old swapchain which will be recreated, so that
 * resources to not need to be destroyed and created. Currently unimplemented.
 * 
 * @return a SpiritSwapchain object which can be used in the program. If there is a
 * failure, it will return NULL. Always check if the result it NULL.
 * 
 * @author Kael Johnston
 */
SpiritSwapchain spCreateSwapchain (
    SpiritSwapchainCreateInfo createInfo,
    SpiritDevice              device, 
    SpiritSwapchain           optionalSwapchain);

/**
 * Submit commands which have been recorded in a command buffer
 * to the GPU. This should be used after recording commands using a material,
 * or using the functions in the context.
 * 
 * @param device must be a valid SpiritDevice, created using spCreateDevice()
 * @param swapchain must be a valid SpiritSwapchain, created using spCreateSwapchain()
 * @param buffer must be a valid VkCommandBuffer, created using vkAllocateCommandBuffers()
 * @param imageIndex the swapchain image for the image to be rendered to.
 * It can be gotten using spSwapchainAquireNextImage
 * 
 * @return SPIRIT_SUCCESS if the command buffer was submit successfully, or 
 * SPIRIT_FAILURE otherwise.
 * 
 * @author Kael Johnston
 */
SpiritResult spSwapchainSubmitCommandBuffer(
    SpiritDevice device,
    SpiritSwapchain swapchain,
    VkCommandBuffer buffer,
    u32 imageIndex);

/**
 * Aquire the next image to render to. This function should be used with
 * spSwapchainSubmitCommandBuffer() to draw stuff. Run this after submitting a frame.
 * 
 * It sets the imageIndex pointer to the next image to draw to, which should be passed
 * to the imageIndex parameter of spSwapchainSubmitCommandBuffer
 * 
 * @param device must be a valid SpiritDevice, created using spCreateDevice()
 * @param swapchain must be a valid SpiritSwapchain, created using spCreateSwapchain()
 * @param imageIndex must be a valid pointer to a u32, which will be set to the index
 * of the next swapchain image
 * 
 * @return SPIRIT_SUCCESS if the image was aquired successfully, otherwise returns
 * SPIRIT_FAILURE. If the function fails, the imageIndex it provides should not be used.
 * 
 * @author Kael Johnston
 */
SpiritResult spSwapchainAquireNextImage(
    const SpiritDevice device,
    const SpiritSwapchain swapchain,
    u32 *imageIndex);

/**
 * Used to add framebuffers to a swapchain. This must be done once a
 * render pass has been created, which requires a swapchain. The framebuffers are 
 * added seperatel to make this possible. 
 * 
 * The swapchain currently only supports 1 render pass at a time, but the framebuffers
 * can be switched by calling the function again on the same swapchain with a
 * different render pass. This should not be done on a per-frame basis, because
 * it requires the framebuffer resources be created and destroyed.
 * 
 * @param device must be a valid SpiritDevice, created using spCreateDevice or
 * automatically using a context.
 * @param swapchain must be a valid SpiritSwapchain, with or without existing
 * framebuffers. Created using spCreateSwapchain, or automatically using a context.
 * @param renderPass must be a valid SpiritRenderPass, created using spCreateRenderPass,
 * or automaticaly using a material
 * 
 * @return SPIRIT_SUCCESS if the framebuffers were created, otherwise returns
 * SPIRIT_FAILURE.
 * 
 * @author Kael Johnston
 */
SpiritResult spSwapchainAddFramebuffers(
    const SpiritDevice device,
    SpiritSwapchain swapchain,
    const SpiritRenderPass renderPass);

/**
 * Destroy a swapchain object. This will cause errors if
 * every object that depends on the swapchain is not already destroyed, and may
 * cause segmentation faults or double frees.
 * 
 * @param swapchain the SpiritSwapchain to destroy
 * @param device a valid SpiritDevice. If you got this far, I hope you know
 * how to create one.
 * 
 * @return SPIRIT_SUCCESS if the swapchain was successfuly destroyed, SPIRIT_FAILURE
 * otherwise. Failure does not mean the swapchain was partialy destroyed, so it should
 * not be used after this call no matter what.
 * 
 * @author Kael Johnston
 */
SpiritResult spDestroySwapchain(
    SpiritSwapchain swapchain, 
    const SpiritDevice device);
