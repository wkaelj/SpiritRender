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

//
// Public functions
//

// create swapchain instance
SpiritSwapchain spCreateSwapchain (SpiritSwapchainCreateInfo createInfo, SpiritDevice device, SpiritSwapchain optionalSwapchain) {
    
    db_assert(device, "Device cannot be null when creating swapchain");

    // set present and format to fallback values
    if (!createInfo.selectedFormat) {
        createInfo.preferedFormat.format = VK_FORMAT_R8G8B8_SRGB;
        createInfo.preferedFormat.colorSpace = VK_COLOR_SPACE_SRGB_NONLINEAR_KHR;
    }
    if (!createInfo.selectedPresentMode) {
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
    u32 swapImageCount = device->swapchainDetails.capabilties.minImageCount + 1;
    if (device->swapchainDetails.capabilties.maxImageCount > 0 &&
    swapImageCount > device->swapchainDetails.capabilties.maxImageCount) {
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
    out->format = chooseSwapSurfaceFormat(
        device->swapchainDetails.formatCount, 
        device->swapchainDetails.formats, 
        createInfo.preferedFormat);
    out->supportInfo = device->swapchainDetails;

    swapInfo.imageExtent = out->extent;
    swapInfo.imageFormat = out->format.format;
    swapInfo.imageColorSpace= out->format.colorSpace;
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

    // images
    vkGetSwapchainImagesKHR(
        device->device,
        out->swapchain, 
        &out->imageCount, 
        NULL);
    // allocate new array of images, and populate it
    out->images = new_array(VkImage, out->imageCount);
    vkGetSwapchainImagesKHR(
        device->device,
        out->swapchain,
        &out->imageCount,
        out->images);

    // image views
    VkImageViewCreateInfo imageViewInfo = {};
    imageViewInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;

    imageViewInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
    imageViewInfo.format = out->format.format;
    imageViewInfo.components.r = VK_COMPONENT_SWIZZLE_IDENTITY;
    imageViewInfo.components.g = VK_COMPONENT_SWIZZLE_IDENTITY;
    imageViewInfo.components.b = VK_COMPONENT_SWIZZLE_IDENTITY;
    imageViewInfo.components.a = VK_COMPONENT_SWIZZLE_IDENTITY;

    imageViewInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
    imageViewInfo.subresourceRange.baseMipLevel = 0;
    imageViewInfo.subresourceRange.levelCount = 1;
    imageViewInfo.subresourceRange.baseArrayLayer = 0;
    imageViewInfo.subresourceRange.layerCount = 1;
    out->imageCount = swapImageCount;
    out->imageViews = new_array(VkImageView, out->imageCount);

    // cannot overflow imageviews, because imageviews was initialized
    // with out->imageCount, which is what we count against
    for (u8 i = 0; i < out->imageCount; i++) {
        imageViewInfo.image = out->images[i];
        if (vkCreateImageView (
            device->device, // vulkan device
            &imageViewInfo, // ptr to stack address
            NULL,    // nullptr
            &out->imageViews[i] // ptr to stack adress
            )) {
            log_error("Failed to create swapchain image views");
            return NULL;
        }
    }

    log_verbose("Created Swapchain");
    return out;

}

// destroy swapchain instance
SpiritResult spDestroySwapchain (SpiritSwapchain swapchain, SpiritDevice device) {

    // if (swapchain == NULL || device == NULL) {
    //     if (swapchain == NULL) log_error("Cannot destroy swapchain, swapchain is NULL");
    //     if (device == NULL) log_error("Cannot destroy swapchain, device is NULL");
    //     return SPIRIT_FAILURE;
    // }
    db_assert(swapchain != NULL, "swapchain cannot be null <- spDestroySwapchain()");
    db_assert(device != NULL, "device cannot be null <- spDestroySwapchain()");

    vkDestroySwapchainKHR(device->device, swapchain->swapchain, NULL);

    for (u32 i = 0; i < swapchain->imageCount; i++) {
        vkDestroyImageView(device->device, swapchain->imageViews[i], NULL);
    }

    swapchain->imageViews = NULL;
    swapchain->images = NULL;
    swapchain->swapchain = NULL;

    dalloc(swapchain);

    return SPIRIT_SUCCESS;
}

//
// Helper implementation
//

// swapchain
VkSurfaceFormatKHR chooseSwapSurfaceFormat (uint32_t formatCount, const VkSurfaceFormatKHR *availableFormats, VkSurfaceFormatKHR preferedFormat) {

    for (uint32_t i = 0; i < formatCount; i++) {
        if (availableFormats[i].format == preferedFormat.format && availableFormats[i].colorSpace == preferedFormat.colorSpace) {
            return availableFormats[i];
        }
    }
    return availableFormats[0];
}

VkPresentModeKHR chooseSwapPresentMode (uint32_t presentModeCount, const VkPresentModeKHR *availablePresentModes, VkPresentModeKHR preferredPresentMode) {

    // return power saving present mode if using integrated GPU
    // FIXME if (preferIntegratedGPU) { return VK_PRESENT_MODE_FIFO_KHR; }

    // check if prefered render mode is available
    for (uint32_t i = 0; i < presentModeCount; i++) {
        if (availablePresentModes[i] == preferredPresentMode) {
            return availablePresentModes[i];
        }
    }

    // fallback present mode
    return VK_PRESENT_MODE_FIFO_KHR;
}
