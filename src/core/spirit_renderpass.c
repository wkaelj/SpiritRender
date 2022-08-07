#include "spirit_renderpass.h"
#include "core/spirit_types.h"

// Renderpass implementation
//
//
// Kael Johnston March 22 2022


VkRenderPass createRenderPass(
    SpiritRenderPassCreateInfo *createInfo,
    const SpiritDevice device,
    const SpiritSwapchain swapchain);

SpiritResult createFramebuffers(
    const SpiritDevice device,
    const SpiritSwapchain swapchain,
    SpiritRenderPass renderPass);

void destroyFrameBuffers(const SpiritDevice device, SpiritRenderPass renderPass);


SpiritRenderPass spCreateRenderPass (
    SpiritRenderPassCreateInfo *createInfo, 
    const SpiritDevice          device,
    const SpiritSwapchain       swapchain) {

    SpiritRenderPass out = new_var(struct t_SpiritRenderPass);

    out->renderPass = createRenderPass(createInfo, device, swapchain);
    out->framebufferCount = 0;
    out->framebuffers = NULL;

    createFramebuffers(device, swapchain, out);

    return out;
}

SpiritResult spRenderPassRecreateFramebuffers(
    const SpiritDevice device,
    SpiritRenderPass renderPass,
    const SpiritSwapchain swapchain)
{
    return createFramebuffers(device, swapchain, renderPass);
}

SpiritResult spDestroyRenderPass (
    SpiritRenderPass renderPass, 
    SpiritDevice     device)
{

    destroyFrameBuffers(device, renderPass);

    if (renderPass == NULL) return SPIRIT_FAILURE;
    vkDestroyRenderPass(device->device, renderPass->renderPass, NULL);

    free(renderPass);

    return SPIRIT_SUCCESS;
}

//
// Helpers
//

VkRenderPass createRenderPass(
    SpiritRenderPassCreateInfo *createInfo,
    const SpiritDevice device,
    const SpiritSwapchain swapchain)
{
    VkAttachmentDescription depthAttachment = {};
    depthAttachment.format = swapchain->depthFormat;
    depthAttachment.samples = VK_SAMPLE_COUNT_1_BIT;
    depthAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
    depthAttachment.storeOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
    depthAttachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
    depthAttachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
    depthAttachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
    depthAttachment.finalLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;

    VkAttachmentReference depthAttachmentRef = {};
    depthAttachmentRef.attachment = 1;
    depthAttachmentRef.layout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;

    VkAttachmentDescription colorAttachment = {};
    colorAttachment.format = swapchain->imageFormat;
    colorAttachment.samples = VK_SAMPLE_COUNT_1_BIT;
    colorAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
    colorAttachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
    colorAttachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
    colorAttachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
    colorAttachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
    colorAttachment.finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;

    VkAttachmentReference colorAttachmentRef = {};
    colorAttachmentRef.attachment = 0;
    colorAttachmentRef.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

    VkSubpassDescription subpass = {
        .pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS,
        .colorAttachmentCount = 1,
        .pColorAttachments = &colorAttachmentRef,
        .pDepthStencilAttachment = &depthAttachmentRef
    };


    VkSubpassDependency dependency = {
        .dstSubpass = 0,
        .dstAccessMask =
            VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT | 
            VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT,
        .dstStageMask =
            VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT | 
            VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT,
        .srcSubpass = VK_SUBPASS_EXTERNAL,
        .srcAccessMask = 0,
        .srcStageMask =
            VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT | 
            VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT
    };


    VkAttachmentDescription attachments[2] = {colorAttachment, depthAttachment};

    // create info
    VkRenderPassCreateInfo renderPassInfo = {
        .sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO,
        .attachmentCount = 2,
        .pAttachments = attachments,
        .subpassCount = 1,
        .pSubpasses = &subpass,
        .dependencyCount = 1,
        .pDependencies = &dependency
    };

    


    VkRenderPass renderPass;
    if (vkCreateRenderPass(
        device->device, 
        &renderPassInfo, 
        NULL, 
        &renderPass)) return NULL;
    log_verbose("Created render pass");

    return renderPass;
}

SpiritResult createFramebuffers(
    const SpiritDevice device,
    const SpiritSwapchain swapchain,
    SpiritRenderPass renderPass)
{

    if (renderPass->framebufferCount && 
        renderPass->framebufferCount != swapchain->imageCount)
    {
        log_fatal("Frambuffer image count changed");
        abort();
    }

    // destroy existing framebuffers
    if(renderPass->framebuffers)
    {
        for (u32 i = 0; i < renderPass->framebufferCount; i++)
        {
            vkDestroyFramebuffer(
                device->device, 
                renderPass->framebuffers[i], 
                NULL);
        }

    }

    // allocate memory to fit framebuffers
    renderPass->framebuffers = new_array(VkFramebuffer, swapchain->imageCount);
    renderPass->framebufferCount = swapchain->imageCount;

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
            &renderPass->framebuffers[i]) != VK_SUCCESS)
        {
            log_error("Failed to add framebuffers to swapchain");
            // destroy already created swapchains
            for (size_t x = 0; x < i; x++)
            {
                vkDestroyFramebuffer(
                    device->device, 
                    renderPass->framebuffers[x], 
                    NULL);
            }

            free(renderPass->framebuffers);

            return SPIRIT_FAILURE;
        }
    }


    return SPIRIT_SUCCESS;
}

void destroyFrameBuffers(const SpiritDevice device, SpiritRenderPass renderPass)
{
    // destroy existing framebuffers
    if(renderPass->framebuffers)
    {
        for (u32 i = 0; i < renderPass->framebufferCount; i++)
        {
            vkDestroyFramebuffer(
                device->device, 
                renderPass->framebuffers[i], 
                NULL);

        }

        free(renderPass->framebuffers);
    }
}

