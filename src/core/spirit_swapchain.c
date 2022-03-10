#include "spirit_swapchain.h"

// swapchain implementation

#define log_verbose(messege, ...) if(createInfo.verbose) LOG_INFO(messege, ##__VA_ARGS__)

//
// Structs
//

//
// Helper functions
//

// choose desired or best swap chain mode
VkSurfaceFormatKHR chooseSwapSurfaceFormat (uint32_t formatCount, const VkSurfaceFormatKHR *availableFormats, VkSurfaceFormatKHR preferedFormat);


// choose best or desired surface format
VkPresentModeKHR chooseSwapPresentMode (uint32_t presentModeCount, const VkPresentModeKHR *availablePresentModes, VkPresentModeKHR preferredPresentMode);
//
// Public functions
//

SpiritSwapchain spCreateSwapchain (SpiritSwapchainCreateInfo createInfo, SpiritDevice device) {

    // failure
    if (device == SPIRIT_NULL) {
        LOG_ERROR("device == NULL, cannot create swapchain");
        return SPIRIT_NULL;
    }

    // set present and format to fallback values
    if (!createInfo.selectedFormat) {
        createInfo.preferedFormat.format = VK_FORMAT_R8G8B8_SRGB;
        createInfo.preferedFormat.colorSpace = VK_COLOR_SPACE_SRGB_NONLINEAR_KHR;
    }
    if (!createInfo.selectedPresentMode) {
        createInfo.preferredPresentMode = VK_PRESENT_MODE_FIFO_KHR;
    }

    // clamp window resolution to capabilties
    clamp_value(createInfo.windowWidthPx,
            device->swapchainDetails.capabilties.minImageExtent.width,
            device->swapchainDetails.capabilties.maxImageExtent.width);
    clamp_value(createInfo.windowHeightPx,
            device->swapchainDetails.capabilties.minImageExtent.height,
            device->swapchainDetails.capabilties.maxImageExtent.height);
    log_verbose("Window resolution '%ix%i'", createInfo.windowWidthPx, createInfo.windowHeightPx);

    SpiritSwapchain out = new_var(SpiritSwapchain);

    VkSwapchainCreateInfoKHR swapInfo = {};
    swapInfo.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;

    // image count
    u32 swapImageCount = device->swapchainDetails.capabilties.minImageCount + 1;
    if (device->swapchainDetails.capabilties.maxImageCount > 0 && swapImageCount > device->swapchainDetails.capabilties.maxImageCount) {
        swapImageCount = device->swapchainDetails.capabilties.maxImageCount;
    }
    swapInfo.minImageCount = swapImageCount;

    // output info
    out->extent = (VkExtent2D){ createInfo.windowWidthPx, createInfo.windowHeightPx };
    out->presentMode = chooseSwapPresentMode(device->swapchainDetails.presentModeCount,
            device->swapchainDetails.presentModes,
            createInfo.preferredPresentMode);
    out->format = chooseSwapSurfaceFormat(device->swapchainDetails.formatCount, device->swapchainDetails.formats, createInfo.preferedFormat);
    out->supportInfo = device->swapchainDetails;

    swapInfo.imageExtent = out->extent;
    swapInfo.imageFormat = out->format.format;
    swapInfo.imageColorSpace= out->format.colorSpace;
    swapInfo.imageExtent = out->extent;
    swapInfo.imageArrayLayers = 1; // FIXME
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

    if (vkCreateSwapchainKHR(device->device, &swapInfo, SPIRIT_NULL, &out->swapchain)) {
        log_verbose("Created Swapchain");
        return SPIRIT_NULL;
    }

    return out;

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
