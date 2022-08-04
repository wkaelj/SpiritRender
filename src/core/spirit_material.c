#include "spirit_material.h"

#include "spirit_renderpass.h"
#include "spirit_pipeline.h"
#include "spirit_context.h"
#include "spirit_mesh.h"

//
// Structures
//

//
// Helper functions
//
SpiritResult beginRenderPass(
    VkCommandBuffer buffer,
    const u32 index,
    const SpiritSwapchain swapchain, 
    const SpiritRenderPass renderPass);

void endRenderPass(VkCommandBuffer buffer);

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
        free(material);
        log_error("Failed to make render pass for material '%s'", createInfo->name);
        return NULL;
    }

    // create associated pipeline
    SpiritPipelineCreateInfo pipelineCreateInfo = {};
    pipelineCreateInfo.vertexShader = createInfo->vertexShader;
    pipelineCreateInfo.fragmentShader = createInfo->fragmentShader;

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
        free(material);
        log_error("Failed to make pipeline for material '%s'", createInfo->name);
        return NULL;
    }

    LIST_INIT(&material->meshList);

    return material;

}

SpiritResult spMaterialUpdate(
    const SpiritContext context, 
    SpiritMaterial material)
{

    return spRenderPassRecreateFramebuffers(
        context->device, 
        material->renderPass, 
        context->swapchain);
}

SpiritResult spMaterialAddMesh(
    SpiritMaterial material,
    const SpiritMeshReference meshRef)
{
    struct t_MaterialListNode *newNode = new_var(struct t_MaterialListNode);
    newNode->mesh = spCheckoutMesh(meshRef);

    LIST_INSERT_HEAD(&material->meshList, newNode, data);
    material->meshCount++;
    return SPIRIT_SUCCESS;
}

size_t spMaterialRecordCommands(
    const SpiritContext context,
    SpiritMaterial material)
{
    db_assert(context->isRecording, 
    "Context is not recording, cannot record commands");

    if(beginRenderPass(
        context->commandBuffers[context->commandBufferIndex],
        context->commandBufferIndex,
        context->swapchain,
        material->renderPass))
    {
        return SPIRIT_FAILURE;
    }

    // configure dynamic state
    VkViewport viewport = {
        .x = 0.0f,
        .y = 0.0f,
        .width = (float) context->screenResolution.w,
        .height = (float) context->screenResolution.h,
        .minDepth = 0.0f,
        .maxDepth = 1.0f
    };

    VkRect2D scissor = {
        {0, 0},
        {context->screenResolution.w, context->screenResolution.h}
    };

    vkCmdSetViewport(
        context->commandBuffers[context->commandBufferIndex],
        0, 1,
        &viewport);
    vkCmdSetScissor(
        context->commandBuffers[context->commandBufferIndex],
        0, 1,
        &scissor);

    spPipelineBindCommandBuffer(
        material->pipeline, 
        context->commandBuffers[context->commandBufferIndex]);

    // iterate through meshes and submit vertexes
    struct t_MaterialListNode *currentMesh = material->meshList.lh_first;
    while(currentMesh != NULL)
    {
        VkBuffer vertBuffers[] = 
        {
            spMeshManagerAccessMesh(currentMesh->mesh)->vertexBuffer
        };
        VkDeviceSize offsets[] = { 0 };

        vkCmdBindVertexBuffers(
            context->commandBuffers[context->commandBufferIndex],
            0,
            1,
            vertBuffers,
            offsets);

        vkCmdDraw(
            context->commandBuffers[context->commandBufferIndex],
            currentMesh->mesh.vertCount,
            1, 0, 0);

        // move to next list element and remove processed element
        struct t_MaterialListNode *oldNode = currentMesh;
        currentMesh = LIST_NEXT(currentMesh, data);
        spReleaseMesh(oldNode->mesh);
        LIST_REMOVE(oldNode, data);
        free(oldNode);
    }

    endRenderPass(context->commandBuffers[context->commandBufferIndex]);

    return SPIRIT_SUCCESS;

}

SpiritResult spDestroyMaterial(
    const SpiritContext context,
    SpiritMaterial material)
{
    spDestroyPipeline(context->device, material->pipeline);
    spDestroyRenderPass(material->renderPass, context->device);
    free(material);
    return SPIRIT_SUCCESS;
}

//
// Command buffer and render pass helper functions
//

SpiritResult beginRenderPass(
    VkCommandBuffer buffer,
    const u32 index,
    const SpiritSwapchain swapchain, 
    const SpiritRenderPass renderPass)
{
    
    VkRenderPassBeginInfo renderPassBeginInfo = {};
    renderPassBeginInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
    renderPassBeginInfo.renderPass = renderPass->renderPass;
    renderPassBeginInfo.framebuffer = renderPass->framebuffers[index];

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
