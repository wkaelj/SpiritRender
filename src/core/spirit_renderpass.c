#include "spirit_renderpass.h"

// Renderpass implementation
//
//
// Kael Johnston March 22 2022

SpiritRenderPass spCreateRenderPass (SpiritRenderPassCreateInfo createInfo, SpiritDevice device, SpiritSwapchain swapchain) {

    SpiritRenderPass out = new_var(SpiritRenderPass);

    // colour attachment
    VkAttachmentDescription attachmentInfo = {};
    attachmentInfo.format = swapchain->format.format;
    attachmentInfo.samples = VK_SAMPLE_COUNT_1_BIT;

    attachmentInfo.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
    attachmentInfo.storeOp = VK_ATTACHMENT_STORE_OP_STORE;

    attachmentInfo.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE; // FIXME
    attachmentInfo.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE; // FIXME

    attachmentInfo.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
    attachmentInfo.finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;

    VkAttachmentReference attachmentRef = {};
    attachmentRef.attachment = 0;
    attachmentRef.layout = VK_IMAGE_LAYOUT_ATTACHMENT_OPTIMAL;





    return out;
}
