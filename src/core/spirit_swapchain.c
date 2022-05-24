#include "spirit_swapchain.h"
#include "core/spirit_fence.h"
#include "spirit_image.h"


//
// Helper functions
//

// choose desired or best swap chain mode
VkSurfaceFormatKHR chooseSwapSurfaceFormat (
    uint32_t                  formatCount,
    const VkSurfaceFormatKHR *availableFormats,
    VkSurfaceFormatKHR        preferedFormat);


// choose best or desired surface format
VkPresentModeKHR chooseSwapPresentMode (
    uint32_t                presentModeCount,
    const VkPresentModeKHR *availablePresentModes,
    VkPresentModeKHR        preferredPresentMode);

SpiritResult createImages(const SpiritDevice device, SpiritSwapchain swapchain);
SpiritResult createDepthObjects(const SpiritDevice device, SpiritSwapchain swapchain);

void destroyDepthObjects(const SpiritDevice device, SpiritSwapchain swapchain);
void destroyImages(const SpiritDevice device, SpiritSwapchain swapchain);



VkFormat findDepthFormat(const SpiritDevice device);
//
// Public functions
//

// create swapchain instance
SpiritSwapchain spCreateSwapchain (
        SpiritSwapchainCreateInfo *createInfo,
        SpiritDevice device,
        SpiritSwapchain optionalSwapchain)
{

    db_assert_msg(device, "Device cannot be NULL when creating swapchain");

    // set present and format to fallback values
    if (!createInfo->selectedFormat)
    {
        createInfo->preferedFormat.format = VK_FORMAT_R8G8B8_SRGB;
        createInfo->preferedFormat.colorSpace = VK_COLOR_SPACE_SRGB_NONLINEAR_KHR;
    }
    if (device->powerSaveMode)
    {
        createInfo->preferredPresentMode = VK_PRESENT_MODE_FIFO_KHR;
    }
    else if (!createInfo->selectedPresentMode)
    {
        createInfo->preferredPresentMode = VK_PRESENT_MODE_FIFO_KHR;
    }

    spDeviceUpdateSwapchainSupport(device);

    // clamp window resolution to capabilties
    createInfo->windowRes.w = clamp_value(
        createInfo->windowRes.w,
        device->swapchainDetails.capabilties.minImageExtent.width,
        device->swapchainDetails.capabilties.maxImageExtent.width);
    createInfo->windowRes.h = clamp_value(
        createInfo->windowRes.h,
        device->swapchainDetails.capabilties.minImageExtent.height,
        device->swapchainDetails.capabilties.maxImageExtent.height);

    if (!optionalSwapchain)
    {
        log_verbose("Window resolution '%lux%lu'",
            createInfo->windowRes.w,
            createInfo->windowRes.h);
    }

    // use old swapchain memory to save memory
    SpiritSwapchain out;
    if (optionalSwapchain)
    {
        destroyDepthObjects(device, optionalSwapchain);
        destroyImages(device, optionalSwapchain);
        out = optionalSwapchain;
    } else
        out = new_var(struct t_SpiritSwapchain);

    VkSwapchainCreateInfoKHR swapInfo = {};
    swapInfo.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;

    // image count
    const u32 maxImageCount = device->swapchainDetails.capabilties.maxImageCount;
    const u32 minImageCount = device->swapchainDetails.capabilties.minImageCount;
    const u32 swapImageCount = clamp_value(3, minImageCount, maxImageCount);

    swapInfo.minImageCount = swapImageCount;

    // output info
    VkExtent2D outExtent = {
        createInfo->windowRes.w,
        createInfo->windowRes.h
    };
    out->extent = outExtent;

    // present mode
    out->presentMode = chooseSwapPresentMode(
        device->swapchainDetails.presentModeCount,
        device->swapchainDetails.presentModes,
        createInfo->preferredPresentMode);
    // surface format
    out->surfaceFormat = chooseSwapSurfaceFormat(
        device->swapchainDetails.formatCount,
        device->swapchainDetails.formats,
        createInfo->preferedFormat);
    out->supportInfo = device->swapchainDetails;

    swapInfo.imageExtent = out->extent;
    swapInfo.imageFormat = out->surfaceFormat.format;
    swapInfo.imageColorSpace= out->surfaceFormat.colorSpace;
    swapInfo.imageExtent = out->extent;
    swapInfo.imageArrayLayers = 1;
    swapInfo.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;

    swapInfo.surface = device->windowSurface;

    if (device->graphicsQueue != device->presentQueue) {
        swapInfo.imageSharingMode = VK_SHARING_MODE_CONCURRENT;
        swapInfo.queueFamilyIndexCount = device->queueCount;
        swapInfo.pQueueFamilyIndices = device->queueIndices;
    } else {
        swapInfo.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
        swapInfo.queueFamilyIndexCount = 0; // Optional
        swapInfo.pQueueFamilyIndices = NULL; // Optional
    }

    swapInfo.preTransform = device->swapchainDetails.capabilties.currentTransform;
    swapInfo.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
    swapInfo.clipped = VK_TRUE;


    // old swapchain
    if (optionalSwapchain) swapInfo.oldSwapchain = optionalSwapchain->swapchain;

    if (vkCreateSwapchainKHR(device->device, &swapInfo, NULL, &out->swapchain)) {
        log_error("Failed to create swapchain");
        free(optionalSwapchain);
        return NULL;
    }

    out->imageCount = 0;

    // images
    if (createImages(device, out))
    {
        log_error("Failed to create images");
        return NULL;
    }

    db_assert_msg(out->imageCount != 0, "Swapchain must have images");
    if (createDepthObjects(device, out))
    {
        log_error("Failed to create depth objects");
        return NULL;
    }

    // store create info
    out->createInfo = *createInfo;

    if (!optionalSwapchain)
    {
        log_verbose("Created Swapchain, image count %u", out->imageCount);
    }

    return out;

} // spCreateSwapchain

SpiritResult spSwapchainPresent(
    const SpiritDevice device,
    const SpiritSwapchain swapchain,
    const VkSemaphore waitSemaphore,
    const u32 imageIndex)
{
    VkPresentInfoKHR presentInfo = {};
    presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;

    presentInfo.waitSemaphoreCount = 1;
    presentInfo.pWaitSemaphores = &waitSemaphore;

    VkSwapchainKHR swapchains[] = { swapchain->swapchain };
    presentInfo.swapchainCount = 1;
    presentInfo.pSwapchains = swapchains;

    presentInfo.pImageIndices = &imageIndex;

    if (vkQueuePresentKHR(device->presentQueue, &presentInfo))
    {
        return SPIRIT_FAILURE;
    }

    return SPIRIT_SUCCESS;
}


// get the next image to render too
SpiritResult spSwapchainAquireNextImage(
    const SpiritDevice device,
    const SpiritSwapchain swapchain,
    VkSemaphore waitSemaphore,
    u32 *imageIndex)
{

    db_assert_msg(device, "Must have a valid devce");
    db_assert_msg(swapchain, "Must have a valid swapchain");
    db_assert_msg(imageIndex, "imageIndex must be a valid pointer to a u32");

    if (vkAcquireNextImageKHR(
        device->device,
        swapchain->swapchain,
        UINT64_MAX,
        waitSemaphore,
        VK_NULL_HANDLE,
        imageIndex))
    {
        log_warning("Error attempting to aquire next image");
        return SPIRIT_FAILURE;
    }

    return SPIRIT_SUCCESS;
}

// destroy swapchain instance
SpiritResult spDestroySwapchain (SpiritSwapchain swapchain, const SpiritDevice device) {

    db_assert_msg(swapchain != NULL, "swapchain cannot be NULL");
    db_assert_msg(device != NULL, "device cannot be NULL");

    spDeviceWaitIdle(device);

    destroyDepthObjects(device, swapchain);
    destroyImages(device, swapchain);

    vkDestroySwapchainKHR(device->device, swapchain->swapchain, NULL);


    free(swapchain);

    return SPIRIT_SUCCESS;
}

//
// Helper implementation
//

SpiritResult createImages(const SpiritDevice device, SpiritSwapchain swapchain)
{

    // images
    vkGetSwapchainImagesKHR(
        device->device,
        swapchain->swapchain,
        &swapchain->imageCount,
        NULL);

    if (swapchain->imageCount > 3)
        log_warning("Unexpected image count %u", swapchain->imageCount);

    // allocate new array of images, and populate it
    VkImage imageBuf[swapchain->imageCount];
    vkGetSwapchainImagesKHR(
        device->device,
        swapchain->swapchain,
        &swapchain->imageCount,
        imageBuf);

    if (imageBuf[0] == NULL)
    {
        return SPIRIT_FAILURE;
    }

    swapchain->images = new_array(SpiritImage, swapchain->imageCount);

    // convert swapchain images to SpiritImages, and create image views
    for (u32 i = 0; i < swapchain->imageCount; ++i)
    {
        SpiritImage *image = &swapchain->images[i];
        *image = (struct t_SpiritImage) {
            .image = imageBuf[i],
            .imageFormat = swapchain->surfaceFormat.format,
            .memory = NULL,
            .aspectFlags = VK_IMAGE_ASPECT_COLOR_BIT,
            .size.w = swapchain->extent.width,
            .size.h = swapchain->extent.height
        };

        spCreateImageView(device, image);
    }

    return SPIRIT_SUCCESS;
}

void destroyImages(const SpiritDevice device, SpiritSwapchain swapchain)
{
    for (u32 i = 0; i < swapchain->imageCount; i++)
    {
        spDestroyImageView(device, &swapchain->images[i]);
    }

    free(swapchain->images);
}

SpiritResult createDepthObjects(const SpiritDevice device, SpiritSwapchain swapchain)
{
    VkFormat depthFormat = findDepthFormat(device);
    VkExtent2D swapChainExtent = swapchain->extent;

    swapchain->depthImages =
        new_array(SpiritImage, swapchain->imageCount);

    for (u32 i = 0; i < swapchain->imageCount; i++)
    {

        SpiritImageCreateInfo depthImageInfo = {
            .flags = 0,
            .size.w = swapChainExtent.width,
            .size.h = swapChainExtent.height,
            .format = depthFormat,
            .tiling = VK_IMAGE_TILING_OPTIMAL,
            .usageFlags = VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT,
            .memoryFlags = VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
            .aspectFlags = VK_IMAGE_ASPECT_DEPTH_BIT,
            .mipLevels = 1,
            .withImageView = true
        };

        spCreateImage(device, &depthImageInfo, &swapchain->depthImages[i]);

    }

    return SPIRIT_SUCCESS;
}

void destroyDepthObjects(const SpiritDevice device, SpiritSwapchain swapchain)
{
    for (u32 i = 0; i < swapchain->imageCount; i++)
    {
        spDestroyImage(device, & swapchain->depthImages[i]);
    }

    free(swapchain->depthImages);
}

// swapchain
VkSurfaceFormatKHR chooseSwapSurfaceFormat (
    uint32_t formatCount,
    const VkSurfaceFormatKHR *availableFormats,
    VkSurfaceFormatKHR preferedFormat)
{

    for (uint32_t i = 0; i < formatCount; i++)
    {
        if (
            availableFormats[i].format == preferedFormat.format &&
            availableFormats[i].colorSpace == preferedFormat.colorSpace)
        {
            return availableFormats[i];
        }
    }
    return availableFormats[0];
}


// extra functions
VkPresentModeKHR chooseSwapPresentMode(
    uint32_t presentModeCount,
    const VkPresentModeKHR *availablePresentModes,
    VkPresentModeKHR preferredPresentMode)
{

    bool foundFallback = false;

    // check if prefered render mode is available
    for (uint32_t i = 0; i < presentModeCount; i++)
    {
        if (availablePresentModes[i] == preferredPresentMode)
            return availablePresentModes[i];

        if (availablePresentModes[i] == VK_PRESENT_MODE_FIFO_KHR)
            foundFallback = true;
    }

    if (foundFallback)
        return VK_PRESENT_MODE_FIFO_KHR;
    else
        return availablePresentModes[0];
}

VkFormat findDepthFormat(const SpiritDevice device)
{
    return spDeviceFindSupportedFormat(
        device,
        (VkFormat[]){
            VK_FORMAT_D32_SFLOAT,
            VK_FORMAT_D32_SFLOAT_S8_UINT,
            VK_FORMAT_D24_UNORM_S8_UINT
        },
        3,
        VK_IMAGE_TILING_OPTIMAL,
        VK_FORMAT_FEATURE_DEPTH_STENCIL_ATTACHMENT_BIT);
}
