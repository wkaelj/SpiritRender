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

    VkSubpassDescription subpass = {};
    subpass.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
    subpass.colorAttachmentCount = 1;
    subpass.pColorAttachments = &attachmentRef;

    VkRenderPassCreateInfo renderPassInfo = {};
    renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
    renderPassInfo.attachmentCount = 1;
    renderPassInfo.pAttachments = &attachmentInfo;
    renderPassInfo.subpassCount = 1;
    renderPassInfo.pSubpasses = &subpass;

    if (vkCreateRenderPass(device->device, &renderPassInfo, SPIRIT_NULL, &out->renderPass) != VK_SUCCESS) return SPIRIT_NULL;
    log_verbose("Created render pass\n");

    return out;
}

SpiritResult spDestroyRenderPass (SpiritRenderPass renderPass, SpiritDevice device) {

    if (renderPass == SPIRIT_NULL) return SPIRIT_FAILURE;
    vkDestroyRenderPass(device->device, renderPass->renderPass, SPIRIT_NULL);

    dalloc(renderPass);

     return SPIRIT_SUCCESS;
}
