#include "spirit_material.h"


//
// Public functions
//


SpiritMaterial spCreateMaterial(
    const SpiritContext context,
    const SpiritMaterialCreateInfo *createInfo)
{

    SpiritMaterial material = new_var(struct t_SpiritMaterial);

    // render pass
    SpiritRenderPassCreateInfo renderPassCreateInfo = {};

    material->renderPass = spCreateRenderPass(
            &renderPassCreateInfo, 
            context->device, 
            context->swapchain);
    if (material->renderPass == NULL)
    {
        dalloc(material);
        log_error("Failed to make render pass for material '%s'", createInfo->name);
        return NULL;
    }

    // create associated pipeline
    SpiritPipelineCreateInfo pipelineCreateInfo = {};
    pipelineCreateInfo.shaderFilePathCount = 2;
    SpiritShader shaders[2];
    shaders[0].path = createInfo->fragmentShader;
    shaders[0].type = SPIRIT_SHADER_TYPE_FRAGMENT;
    shaders[1].path = createInfo->vertexShader;
    shaders[1].type = SPIRIT_SHADER_TYPE_VERTEX;
    pipelineCreateInfo.shaderFilePaths = shaders;
    pipelineCreateInfo.resolution = context->screenResolution;

    material->pipeline = spCreatePipeline (
        context->device, 
        &pipelineCreateInfo,
        context->swapchain,
        material->renderPass, 
        NULL);

    if (material->pipeline == NULL)
    {
        spDestroyRenderPass(material->renderPass, context->device);
        dalloc(material);
        log_error("Failed to make pipeline for material '%s'", createInfo->name);
        return NULL;
    }


}

SpiritResult spMaterialAddMesh(
    const SpiritContext context)
{

}

SpiritResult spMaterialRecordCommands(
    const SpiritContext context)
{

}

SpiritResult spDestroyMaterial(
    const SpiritContext context,
    SpiritMaterial material)
{

}

//
// Command buffer and render pass helper functions
//



// command buffer creation and destruction
SpiritResult beginCommandBuffer(
        VkCommandBuffer buffer, 
        SpiritPipeline pipeline)
{

    VkCommandBufferBeginInfo bufferBeginInfo = {};
    bufferBeginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;

    if(vkBeginCommandBuffer(buffer, &bufferBeginInfo) != VK_SUCCESS)
    {
        log_error("Could not begin command buffer");
        return SPIRIT_FAILURE;
    }

    spPipelineBindCommandBuffer(
        pipeline,
        buffer);
    return SPIRIT_SUCCESS;
}

void endCommandBuffer(VkCommandBuffer buffer)
{
    vkEndCommandBuffer(buffer);
}

SpiritResult beginRenderPass(
    VkCommandBuffer buffer,
    const u32 index,
    const SpiritSwapchain swapchain, 
    const SpiritRenderPass renderPass)
{
    
    VkRenderPassBeginInfo renderPassBeginInfo = {};
    renderPassBeginInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
    renderPassBeginInfo.renderPass = renderPass->renderPass;
    db_assert(swapchain->framebuffers, "Did not create swapchain frambuffers");
    renderPassBeginInfo.framebuffer = swapchain->framebuffers[index];

    renderPassBeginInfo.renderArea.offset = (VkOffset2D) {0, 0};
    renderPassBeginInfo.renderArea.extent = swapchain->extent;

    const u32 clearValueCount = 2;
    VkClearValue clearValues[2];
    clearValues[0].color = (VkClearColorValue) {0.1f, 0.1f, 0.1f};
    clearValues[1].depthStencil = (VkClearDepthStencilValue) {1.0f, 0};

    renderPassBeginInfo.clearValueCount = clearValueCount;
    renderPassBeginInfo.pClearValues = clearValues;

    vkCmdBeginRenderPass(
        buffer, 
        &renderPassBeginInfo, 
        VK_SUBPASS_CONTENTS_INLINE);

    return SPIRIT_SUCCESS;
}

void endRenderPass(VkCommandBuffer buffer)
{
    vkCmdEndRenderPass(buffer);
}
