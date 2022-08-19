#include "spirit_swapchain.h"
#include "core/spirit_sync.h"
#include "core/spirit_types.h"
#include "debug/messenger.h"
#include "spirit_header.h"
#include "spirit_image.h"
#include <stdint.h>
#include <vulkan/vulkan_core.h>

// swapchain implementation

//
// Structs
//

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

SpiritResult createSyncObjects(const SpiritDevice, SpiritSwapchain);
SpiritResult createImages(const SpiritDevice device, SpiritSwapchain swapchain);
SpiritResult createDepthObjects(const SpiritDevice device, SpiritSwapchain swapchain);

void destroyDepthObjects(const SpiritDevice device, SpiritSwapchain swapchain);
void destroyImages(const SpiritDevice device, SpiritSwapchain swapchain);
void destroySyncObjects(const SpiritDevice device, SpiritSwapchain swapchain);



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

    db_assert(device, "Device cannot be NULL when creating swapchain");

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
        destroySyncObjects(device, optionalSwapchain);
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
    log_debug("minimagecount %u", swapImageCount);

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
    if (createDepthObjects(device, out))
    {
        log_error("Failed to create depth objects");
        return NULL;
    }
    // sync objects
    if (createSyncObjects(device, out))
    {
        log_error("Failed to create sync objects");
        return NULL;
    }

    out->currentFrame = 0;

    // store create info
    out->createInfo = *createInfo;

    if (!optionalSwapchain)
    {
        log_verbose("Created Swapchain, image count %u", out->imageCount);
    }

    return out;

} // spCreateSwapchain

SpiritResult spSwapchainSubmitCommandBuffer(
    SpiritDevice device,
    SpiritSwapchain swapchain,
    SpiritCommandBuffer buffer,
    u32 imageIndex)
{

    db_assert(device, "Must have a valid device");
    db_assert(swapchain, "must have a valid swapchain");
    db_assert(buffer, "Must have a valid command buffer");


    if (swapchain->imagesInFlight[imageIndex])
    {
        if (spFenceWait(device, *swapchain->imagesInFlight[imageIndex], UINT64_MAX))
        {
            log_fatal("Error wating for swapchain fence %u", imageIndex);
            return SPIRIT_FAILURE;
        }
    }

    swapchain->imagesInFlight[imageIndex] =
        &swapchain->inFlightFences[swapchain->currentFrame];

    spFenceReset(device, swapchain->inFlightFences[swapchain->currentFrame]);

    VkSubmitInfo submitInfo = {};
    submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;

    VkSemaphore waitSemaphores[] = {
        swapchain->imageAvailableSemaphores[swapchain->currentFrame]
    };

    VkPipelineStageFlags waitStages[] = {
        VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT
    };
    submitInfo.waitSemaphoreCount = 1;
    submitInfo.pWaitSemaphores = waitSemaphores;
    submitInfo.pWaitDstStageMask = waitStages;

    submitInfo.commandBufferCount = 1;
    submitInfo.pCommandBuffers = &buffer->handle;

    VkSemaphore signalSemaphores[] = {
        swapchain->queueCompleteSemaphores[swapchain->currentFrame]
    };

    submitInfo.signalSemaphoreCount = 1;
    submitInfo.pSignalSemaphores = signalSemaphores;

    if (vkQueueSubmit(
        device->graphicsQueue,
        1,
        &submitInfo,
        swapchain->inFlightFences[imageIndex]->handle) != VK_SUCCESS)
    {
        log_fatal("Failed to submit graphics queue");
        abort();
    }

    VkPresentInfoKHR presentInfo = {};
    presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;

    presentInfo.waitSemaphoreCount = 1;
    presentInfo.pWaitSemaphores = signalSemaphores;

    VkSwapchainKHR swapchains[] = { swapchain->swapchain };
    presentInfo.swapchainCount = 1;
    presentInfo.pSwapchains = swapchains;

    presentInfo.pImageIndices = &imageIndex;

    VkResult result = vkQueuePresentKHR(device->presentQueue, &presentInfo);

    swapchain->currentFrame = (swapchain->currentFrame + 1) %
        swapchain->maxImagesInFlight;

    if(result != VK_SUCCESS)
    {
        log_warning("Failed to present present queue");
        return SPIRIT_FAILURE;
    }
    return SPIRIT_SUCCESS;
}


// get the next image to render too
SpiritResult spSwapchainAquireNextImage(
    const SpiritDevice device,
    const SpiritSwapchain swapchain,
    u32 *imageIndex)
{

    db_assert(device, "Must have a valid devce");
    db_assert(swapchain, "Must have a valid swapchain");
    db_assert(imageIndex, "imageIndex must be a valid pointer to a u32");

    SpiritResult result = SPIRIT_SUCCESS;
    if (spFenceWait(
        device,
        swapchain->inFlightFences[swapchain->currentFrame],
        UINT64_MAX))
    {
        log_error("Error while waiting for fence %u", swapchain->currentFrame);
        result = SPIRIT_FAILURE;
    }

    if (vkAcquireNextImageKHR(
        device->device,
        swapchain->swapchain,
        UINT64_MAX,
        swapchain->imageAvailableSemaphores[swapchain->currentFrame],
        VK_NULL_HANDLE,
        imageIndex))
    {
        log_warning("Error attempting to aquire next image with semaphore %u", swapchain->currentFrame);
        result = SPIRIT_FAILURE;
    }

    return result;
}

// destroy swapchain instance
SpiritResult spDestroySwapchain (SpiritSwapchain swapchain, const SpiritDevice device) {

    db_assert(swapchain != NULL, "swapchain cannot be NULL");
    db_assert(device != NULL, "device cannot be NULL");

    spDeviceWaitIdle(device);

    destroyDepthObjects(device, swapchain);
    destroySyncObjects(device, swapchain);
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
    swapchain->imageCount = 0;

    // images
    vkGetSwapchainImagesKHR(
        device->device,
        swapchain->swapchain,
        &swapchain->imageCount,
        NULL);

    log_debug("Fetched image count %u", swapchain->imageCount);

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
        image->image = imageBuf[i];
        image->imageFormat = swapchain->surfaceFormat.format;
        image->memory = NULL;
        image->aspectFlags = VK_IMAGE_ASPECT_COLOR_BIT;
        image->size.w = swapchain->extent.width;
        image->size.h = swapchain->extent.height;

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

SpiritResult createSyncObjects(const SpiritDevice device, SpiritSwapchain swapchain) {

    // sync options
    swapchain->maxImagesInFlight = swapchain->imageCount - 1;

    swapchain->imageAvailableSemaphores =
        new_array(VkSemaphore, swapchain->maxImagesInFlight);
    swapchain->queueCompleteSemaphores =
        new_array(VkSemaphore, swapchain->maxImagesInFlight);
    swapchain->inFlightFences =
        new_array(SpiritFence, swapchain->maxImagesInFlight);


    VkSemaphoreCreateInfo semaphoreInfo = {};
    semaphoreInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;

    VkFenceCreateInfo fenceInfo = {};
    fenceInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
    fenceInfo.flags = VK_FENCE_CREATE_SIGNALED_BIT;

    for (size_t i = 0; i < swapchain->maxImagesInFlight; i++) {

        bool failure = false;
        if (vkCreateSemaphore(
            device->device,
            &semaphoreInfo,
            NULL,
            &swapchain->imageAvailableSemaphores[i])) failure = true;
       if (vkCreateSemaphore(
            device->device,
            &semaphoreInfo,
            NULL,
            &swapchain->queueCompleteSemaphores[i])) failure = true;

        if (!(swapchain->inFlightFences[i] = spCreateFence(device, true)))
        {
            failure = true;
        }

        if (failure)
        {
            log_error("Failed to create syncronization objects");
            return SPIRIT_FAILURE;
        }
    }

    swapchain->imagesInFlight = new_array(SpiritFence*, swapchain->imageCount);
    for (u32 i = 0; i < swapchain->imageCount; i++)
    {
        swapchain->imagesInFlight[i] = NULL;
    }

    return SPIRIT_SUCCESS;
}

void destroySyncObjects(const SpiritDevice device, SpiritSwapchain swapchain)
{
    for (u32 i = 0; i < swapchain->maxImagesInFlight; i++)
    {
        vkDestroySemaphore(device->device, swapchain->queueCompleteSemaphores[i],
            NULL);
        vkDestroySemaphore(device->device, swapchain->imageAvailableSemaphores[i],
            NULL);
        spDestroyFence(device, swapchain->inFlightFences[i]);
    }

    free(swapchain->imageAvailableSemaphores);
    free(swapchain->queueCompleteSemaphores);
    free(swapchain->inFlightFences);
    free(swapchain->imagesInFlight);
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

    // check if prefered render mode is available
    for (uint32_t i = 0; i < presentModeCount; i++)
    {
        if (availablePresentModes[i] == preferredPresentMode)
        {
            return availablePresentModes[i];
        }
    }

    // fallback present mode
    return VK_PRESENT_MODE_FIFO_KHR;
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
