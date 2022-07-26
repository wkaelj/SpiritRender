#include "spirit_swapchain.h"

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
        SpiritSwapchainCreateInfo createInfo,
        SpiritDevice device,
        SpiritSwapchain optionalSwapchain)
{
    
    db_assert(device, "Device cannot be NULL when creating swapchain");

    // set present and format to fallback values
    if (!createInfo.selectedFormat)
    {
        createInfo.preferedFormat.format = VK_FORMAT_R8G8B8_SRGB;
        createInfo.preferedFormat.colorSpace = VK_COLOR_SPACE_SRGB_NONLINEAR_KHR;
    }
    if (!createInfo.selectedPresentMode)
    {
        createInfo.preferredPresentMode = VK_PRESENT_MODE_FIFO_KHR;
    }

    // clamp window resolution to capabilties
    createInfo.windowWidthPx = clamp_value(
        createInfo.windowWidthPx,
        device->swapchainDetails.capabilties.minImageExtent.width,
        device->swapchainDetails.capabilties.maxImageExtent.width);
    createInfo.windowHeightPx = clamp_value(
        createInfo.windowHeightPx,
        device->swapchainDetails.capabilties.minImageExtent.height,
        device->swapchainDetails.capabilties.maxImageExtent.height);

    log_verbose("Window resolution '%ix%i'", createInfo.windowWidthPx, createInfo.windowHeightPx);

    SpiritSwapchain out = new_var(struct t_SpiritSwapchain);

    VkSwapchainCreateInfoKHR swapInfo = {};
    swapInfo.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;

    // image count
    u32 swapImageCount = 3;
    log_debug("Swapchain minImageCount + 1 = %u", swapImageCount);
    if (device->swapchainDetails.capabilties.maxImageCount > 0 &&
    swapImageCount > device->swapchainDetails.capabilties.maxImageCount)
    {
        swapImageCount = device->swapchainDetails.capabilties.maxImageCount;
    }
    swapInfo.minImageCount = swapImageCount;

    // output info
    VkExtent2D outExtent = {
        createInfo.windowWidthPx,
        createInfo.windowHeightPx
    };
    out->extent = outExtent;

    // present mode
    out->presentMode = chooseSwapPresentMode(
        device->swapchainDetails.presentModeCount,
        device->swapchainDetails.presentModes,
        createInfo.preferredPresentMode);
    // surface format
    out->surfaceFormat = chooseSwapSurfaceFormat(
        device->swapchainDetails.formatCount, 
        device->swapchainDetails.formats, 
        createInfo.preferedFormat);
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

    // viewport?
    swapInfo.preTransform = device->swapchainDetails.capabilties.currentTransform;
    swapInfo.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
    swapInfo.clipped = VK_TRUE;

    // old swapchain
    if (optionalSwapchain != NULL) swapInfo.oldSwapchain = optionalSwapchain->swapchain;

    // TODO fix swapchain create info
    // out->createInfo = swapInfo;
    // actually created swapchain
    if (vkCreateSwapchainKHR(device->device, &swapInfo, NULL, &out->swapchain)) {
        log_error("Failed to create swapchain");
        return NULL;
    }


    // framebuffers must be NULL
    out->framebufferCount = 0;
    out->framebuffers = NULL;

    // images
    if (createImages(device, out)) return NULL;
    if (createDepthObjects(device, out)) return NULL;
    // sync objects
    if (createSyncObjects(device, out)) return NULL;

    out->currentFrame = 0;

    log_verbose("Created Swapchain, image count %u", out->imageCount);
    return out;

} // spCreateSwapchain

SpiritResult spSwapchainSubmitCommandBuffer(
    SpiritDevice device,
    SpiritSwapchain swapchain,
    VkCommandBuffer buffer,
    u32 imageIndex)
{

    db_assert(device, "Must have a valid device");
    db_assert(swapchain, "must have a valid swapchain");
    db_assert(buffer, "Must have a valid command buffer");

    if (vkWaitForFences(
        device->device, 
        1, 
        &swapchain->fences[imageIndex], 
        VK_TRUE, UINT64_MAX))
    {
        log_fatal("Error wating for swapchain fence %u", imageIndex);
        return SPIRIT_FAILURE;
    }

    VkSubmitInfo submitInfo = {};
    submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;

    VkSemaphore waitSemaphores[] = {
        swapchain->imageAvailableSemaphores[swapchain->currentFrame]
    };

    VkPipelineStageFlags waitStages[] = {VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT};
    submitInfo.waitSemaphoreCount = 1;
    submitInfo.pWaitSemaphores = waitSemaphores;
    submitInfo.pWaitDstStageMask = waitStages;

    submitInfo.commandBufferCount = 1;
    submitInfo.pCommandBuffers = &buffer;

    VkSemaphore signalSemaphores[] = {
        swapchain->renderFinishedSemaphores[swapchain->currentFrame]
    };

    submitInfo.signalSemaphoreCount = 1;
    submitInfo.pSignalSemaphores = signalSemaphores;

    if (vkResetFences(
        device->device, 
        1, 
        &swapchain->fences[imageIndex]))
    {
        log_fatal("Failed to reset fence %u", swapchain->currentFrame);
    }
    
    if (vkQueueSubmit(
        device->graphicsQueue, 
        1, 
        &submitInfo, 
        swapchain->fences[imageIndex]) != VK_SUCCESS)
    {
        log_fatal("Failed to submit graphics queue");
        abort();
    }

    swapchain->currentFence = imageIndex;

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
        swapchain->maxFramesInFlight;
    
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
    if (vkWaitForFences(
        device->device,
        1,
        &swapchain->fences[swapchain->currentFence],
        VK_TRUE,
        UINT64_MAX))
    {
        log_error("Error while waiting for fence %u", swapchain->currentFrame);
        result = SPIRIT_FAILURE;
    }

    if (vkAcquireNextImageKHR(
        device->device,
        swapchain->swapchain,
        UINT64_MAX,
        swapchain->imageAvailableSemaphores[swapchain->currentFrame],  // must be a not signaled semaphore
        VK_NULL_HANDLE,
        imageIndex))
    {
        log_error("Error attempting to aquire next image with semaphore %u", swapchain->currentFrame);
        result = SPIRIT_FAILURE;
    }   

    return result;
}

SpiritResult spSwapchainAddFramebuffers(
    const SpiritDevice device,
    SpiritSwapchain swapchain,
    const SpiritRenderPass renderPass)
{

    // destroy existing framebuffers
    if(swapchain->framebuffers)
    {
        for (u32 i = 0; i < swapchain->framebufferCount; i++)
        {
            vkDestroyFramebuffer(
                device->device, 
                swapchain->framebuffers[i], 
                NULL);
        }

        free(swapchain->framebuffers);
    }

    // allocate memory to fit framebuffers
    swapchain->framebuffers = new_array(VkFramebuffer, swapchain->imageCount);
    swapchain->framebufferCount = swapchain->imageCount;

    for (size_t i = 0; i < swapchain->imageCount; i++)
    {

        VkImageView attachments[2] = { 
            swapchain->imageViews[i],
            swapchain->depthImageViews[i]
        };
        VkFramebufferCreateInfo framebufferInfo = {};
        framebufferInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
        framebufferInfo.renderPass = renderPass->renderPass;
        db_assert(renderPass && renderPass->renderPass, "No renderpass");
        framebufferInfo.attachmentCount = 2;
        framebufferInfo.pAttachments = attachments;
        framebufferInfo.width = swapchain->extent.width;
        framebufferInfo.height = swapchain->extent.height;
        framebufferInfo.layers = 1;

        if (vkCreateFramebuffer(
            device->device,
            &framebufferInfo,
            NULL,
            &swapchain->framebuffers[i]) != VK_SUCCESS)
        {
            log_error("Failed to add framebuffers to swapchain");
            // destroy already created swapchains
            for (size_t x = 0; x < i; x++)
            {
                vkDestroyFramebuffer(
                    device->device, 
                    swapchain->framebuffers[x], 
                    NULL);
            }

            free(swapchain->framebuffers);

            return SPIRIT_FAILURE;
        }
    }


    return SPIRIT_SUCCESS;
}


// destroy swapchain instance
SpiritResult spDestroySwapchain (SpiritSwapchain swapchain, const SpiritDevice device) {

    db_assert(swapchain != NULL, "swapchain cannot be NULL");
    db_assert(device != NULL, "device cannot be NULL");

    destroyDepthObjects(device, swapchain);
    destroySyncObjects(device, swapchain);
    destroyImages(device, swapchain);

    // destroy existing framebuffers
    if(swapchain->framebuffers)
    {
        for (u32 i = 0; i < swapchain->framebufferCount; i++)
        {
            vkDestroyFramebuffer(
                device->device, 
                swapchain->framebuffers[i], 
                NULL);

        }

        free(swapchain->framebuffers);
    }

    vkDestroySwapchainKHR(device->device, swapchain->swapchain, NULL);

    for (u32 i = 0; i < swapchain->imageCount; i++)
    {
        vkDestroyImageView(device->device, swapchain->imageViews[i], NULL);

        // maybe unbind image
        
        vkFreeMemory(device->device, swapchain->depthImageMemory[i], NULL);
    }

    free(swapchain);

    return SPIRIT_SUCCESS;
}

//
// Helper implementation
//

SpiritResult createImages(const SpiritDevice device, SpiritSwapchain swapchain)
{

    swapchain->imageFormat = swapchain->surfaceFormat.format;
    // images
    vkGetSwapchainImagesKHR(
        device->device,
        swapchain->swapchain, 
        &swapchain->imageCount, 
        NULL);
    // allocate new array of images, and populate it
    swapchain->images = new_array(VkImage, swapchain->imageCount);
    vkGetSwapchainImagesKHR(
        device->device,
        swapchain->swapchain,
        &swapchain->imageCount,
        swapchain->images);

    // image views
    VkImageViewCreateInfo imageViewInfo = {};
    imageViewInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;

    imageViewInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
    imageViewInfo.format = swapchain->imageFormat;
    imageViewInfo.components.r = VK_COMPONENT_SWIZZLE_IDENTITY;
    imageViewInfo.components.g = VK_COMPONENT_SWIZZLE_IDENTITY;
    imageViewInfo.components.b = VK_COMPONENT_SWIZZLE_IDENTITY;
    imageViewInfo.components.a = VK_COMPONENT_SWIZZLE_IDENTITY;

    imageViewInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
    imageViewInfo.subresourceRange.baseMipLevel = 0;
    imageViewInfo.subresourceRange.levelCount = 1;
    imageViewInfo.subresourceRange.baseArrayLayer = 0;
    imageViewInfo.subresourceRange.layerCount = 1;
    swapchain->imageCount = swapchain->imageCount;
    swapchain->imageViews = new_array(VkImageView, swapchain->imageCount);

    for (u8 i = 0; i < swapchain->imageCount; i++)
    {
        imageViewInfo.image = swapchain->images[i];
        if (vkCreateImageView (
            device->device,
            &imageViewInfo,
            NULL,
            &swapchain->imageViews[i]))
        {
            log_error("Failed to create swapchain image views");
            return SPIRIT_FAILURE;
        }
    }

    return SPIRIT_SUCCESS;
}

void destroyImages(const SpiritDevice device, SpiritSwapchain swapchain)
{
    for (u32 i = 0; i < swapchain->imageCount; i++)
    {
        vkDestroyImageView(device->device, swapchain->imageViews[i], NULL);
    }

    free(swapchain->images);
    free(swapchain->imageViews);
}

SpiritResult createSyncObjects(const SpiritDevice device, SpiritSwapchain swapchain) {
    
    // sync options
    swapchain->maxFramesInFlight = swapchain->imageCount - 1;

    swapchain->imageAvailableSemaphores = 
        new_array(VkSemaphore, swapchain->maxFramesInFlight);
    swapchain->renderFinishedSemaphores = 
        new_array(VkSemaphore, swapchain->maxFramesInFlight);

    swapchain->fences = new_array(VkFence, swapchain->imageCount);

    
    VkSemaphoreCreateInfo semaphoreInfo = {};
    semaphoreInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;

    VkFenceCreateInfo fenceInfo = {};
    fenceInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
    fenceInfo.flags = VK_FENCE_CREATE_SIGNALED_BIT;

    for (size_t i = 0; i < swapchain->maxFramesInFlight; i++) {

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
            &swapchain->renderFinishedSemaphores[i])) failure = true;

        if (failure)
        {
            log_error("Failed to create syncronization objects");
            return SPIRIT_FAILURE;
        }
    }

    // create fences, 1 per image
    for (u32 i = 0; i < swapchain->imageCount; i++)
    {
        if (vkCreateFence(device->device, &fenceInfo, NULL, &swapchain->fences[i]))
        {
            log_error("Failed to create fence");
            return SPIRIT_FAILURE;
        }
    }

    return SPIRIT_SUCCESS;
}

void destroySyncObjects(const SpiritDevice device, SpiritSwapchain swapchain)
{
    for (u32 i = 0; i < swapchain->maxFramesInFlight; i++)
    {
        vkDestroySemaphore(device->device, swapchain->renderFinishedSemaphores[i],
            NULL);
        vkDestroySemaphore(device->device, swapchain->imageAvailableSemaphores[i],
            NULL);
        vkDestroyFence(device->device, swapchain->fences[i], NULL);
    }

    vkDestroyFence(
        device->device, 
        swapchain->fences[swapchain->maxFramesInFlight], NULL);

    free(swapchain->imageAvailableSemaphores);
    free(swapchain->renderFinishedSemaphores);
    free(swapchain->fences);
}

SpiritResult createDepthObjects(const SpiritDevice device, SpiritSwapchain swapchain)
{
    VkFormat depthFormat = findDepthFormat(device);
    swapchain->depthFormat = depthFormat;
    VkExtent2D swapChainExtent = swapchain->extent;

    swapchain->depthImages = 
        new_array(VkImage, swapchain->imageCount);
    swapchain->depthImageMemory =
        new_array(VkDeviceMemory, swapchain->imageCount);
    swapchain->depthImageViews = 
        new_array(VkImageView, swapchain->imageCount);

    for (u32 i = 0; i < swapchain->imageCount; i++)
    {
        VkImageCreateInfo imageInfo = {};
        imageInfo.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
        imageInfo.imageType = VK_IMAGE_TYPE_2D;
        imageInfo.extent.width = swapChainExtent.width;
        imageInfo.extent.height = swapChainExtent.height;
        imageInfo.extent.depth = 1;
        imageInfo.mipLevels = 1;
        imageInfo.arrayLayers = 1;
        imageInfo.format = depthFormat;
        imageInfo.tiling = VK_IMAGE_TILING_OPTIMAL;
        imageInfo.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
        imageInfo.usage = VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT;
        imageInfo.samples = VK_SAMPLE_COUNT_1_BIT;
        imageInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
        imageInfo.flags = 0;

        spDeviceCreateImage(
            device,
            &imageInfo,
            VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
            &swapchain->depthImages[i],
            &swapchain->depthImageMemory[i]);

        VkImageViewCreateInfo viewInfo = {};
        viewInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
        viewInfo.image = swapchain-> depthImages[i];
        viewInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
        viewInfo.format = depthFormat;
        viewInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_DEPTH_BIT;
        viewInfo.subresourceRange.baseMipLevel = 0;
        viewInfo.subresourceRange.levelCount = 1;
        viewInfo.subresourceRange.baseArrayLayer = 0;
        viewInfo.subresourceRange.layerCount = 1;

        if (vkCreateImageView(
            device->device, 
            &viewInfo, 
            NULL,
            &swapchain->depthImageViews[i]) != VK_SUCCESS)
        {
            return SPIRIT_FAILURE;
        }
    }

    return SPIRIT_SUCCESS;
}

void destroyDepthObjects(const SpiritDevice device, SpiritSwapchain swapchain)
{
    for (u32 i = 0; i < swapchain->imageCount; i++)
    {
        vkDestroyImageView(device->device, swapchain->depthImageViews[i], NULL);
        vkDestroyImage(device->device, swapchain->depthImages[i], NULL);
        vkFreeMemory(device->device, swapchain->depthImageMemory[i], NULL);
    }

    free(swapchain->depthImages);
    free(swapchain->depthImageViews);
    free(swapchain->depthImageMemory);
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

    // return power saving present mode if using integrated GPU
    // FIXME if (preferIntegratedGPU) { return VK_PRESENT_MODE_FIFO_KHR; }

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
