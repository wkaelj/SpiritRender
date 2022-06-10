#include "spirit_renderpass.h"

// Renderpass implementation
//
//
// Kael Johnston March 22 2022

SpiritRenderPass spCreateRenderPass (
    SpiritRenderPassCreateInfo *createInfo, 
    const SpiritDevice          device,
    const SpiritSwapchain       swapchain) {

    SpiritRenderPass out = new_var(struct t_SpiritRenderPass);

    // colour attachment
    VkAttachmentDescription attachmentInfo = {};
    attachmentInfo.format =  swapchain->format.format;
    attachmentInfo.samples = VK_SAMPLE_COUNT_1_BIT;

    attachmentInfo.loadOp =  VK_ATTACHMENT_LOAD_OP_CLEAR;
    attachmentInfo.storeOp = VK_ATTACHMENT_STORE_OP_STORE;

    attachmentInfo.stencilLoadOp =  VK_ATTACHMENT_LOAD_OP_DONT_CARE; //   FIXME
    attachmentInfo.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE; // FIXME

    attachmentInfo.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
    attachmentInfo.finalLayout =   VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;

    // colour attachment
    VkAttachmentReference attachmentRef = {};
    attachmentRef.attachment = 0;
    attachmentRef.layout = VK_IMAGE_LAYOUT_ATTACHMENT_OPTIMAL;

    // create subpass
    VkSubpassDescription subpass = {};
    subpass.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
    subpass.colorAttachmentCount = 1;
    subpass.pColorAttachments = &attachmentRef;

    // actual render pass
    VkRenderPassCreateInfo renderPassInfo = {};
    renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
    renderPassInfo.attachmentCount = 1;
    renderPassInfo.pAttachments = &attachmentInfo;
    renderPassInfo.subpassCount = 1;
    renderPassInfo.pSubpasses = &subpass;

    if (vkCreateRenderPass(device->device, &renderPassInfo, NULL, &out->renderPass) != VK_SUCCESS) return NULL;
    log_verbose("Created render pass\n");

    return out;
}

SpiritResult spDestroyRenderPass (
    SpiritRenderPass renderPass, 
    SpiritDevice     device)
{

    if (renderPass == NULL) return SPIRIT_FAILURE;
    vkDestroyRenderPass(device->device, renderPass->renderPass, NULL);

    dalloc(renderPass);

    return SPIRIT_SUCCESS;
}
