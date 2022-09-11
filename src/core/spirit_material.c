#include "spirit_material.h"

#include "spirit_context.h"
#include "spirit_mesh.h"
#include "spirit_pipeline.h"
#include "spirit_renderpass.h"

#include "spirit_command_buffer.h"

//
// Structures
//

//
// Helper functions
//

SPIRIT_INLINE void clearQueue(SpiritMaterial material)
{

    while (!LIST_EMPTY(&material->meshList))
    {
        struct t_SpiritMaterialListNode *np = LIST_FIRST(&material->meshList);

        db_assert_msg(np->used, "Unused node added to material queue");
        LIST_REMOVE(np, data);
        spReleaseMesh(np->mesh);
        if (np->isBuffer)
            np->used = false;
        else
            free(np);
    }
}

struct t_SpiritMaterialListNode *findNode(const SpiritMaterial material)
{
    struct t_SpiritMaterialListNode *node = NULL;
    if (material->currentBufferSpot < array_length(material->nodeBuffer))
    {
        node = &material->nodeBuffer[material->currentBufferSpot];
        db_assert(node->isBuffer);
        db_assert(node->used == false);
        material->currentBufferSpot++;
        node->used = true;
    }
    else
    {
        node           = new_var(struct t_SpiritMaterialListNode);
        node->isBuffer = false;
        node->used     = true;
        log_warning("Overflowing material node buffer, increase buffer size");
    }

    return node;
}

SpiritResult
releaseNode(SpiritMaterial material, struct t_SpiritMaterialListNode *node)
{
    db_assert(node && node->used);

    if (!node || !node->used)
        return SPIRIT_FAILURE;

    if (node->isBuffer)
    {
        node->used = false;
        material->currentBufferSpot--;
    }
    else
    {
        free(node);
    }

    return SPIRIT_SUCCESS;
}

//
// Public functions
//

SpiritMaterial spCreateMaterial(
    const SpiritContext context, const SpiritMaterialCreateInfo *createInfo)
{

    SpiritMaterial material = new_var(struct t_SpiritMaterial);

    // render pass
    SpiritRenderPassCreateInfo renderPassCreateInfo = {};

    time_function_with_return(
        spCreateRenderPass(
            &renderPassCreateInfo, context->device, context->swapchain),
        material->renderPass);

    if (material->renderPass == NULL)
    {
        free(material);
        log_error(
            "Failed to make render pass for material '%s'", createInfo->name);
        return NULL;
    }

    // create associated pipeline
    SpiritPipelineCreateInfo pipelineCreateInfo = {};
    pipelineCreateInfo.vertexShader             = createInfo->vertexShader;
    pipelineCreateInfo.fragmentShader           = createInfo->fragmentShader;

    pipelineCreateInfo.resolution     = context->screenResolution;
    pipelineCreateInfo.vertexShader   = createInfo->vertexShader;
    pipelineCreateInfo.fragmentShader = createInfo->fragmentShader;
    pipelineCreateInfo.resolution     = context->screenResolution;

    time_function_with_return(
        spCreatePipeline(
            context->device, &pipelineCreateInfo, material->renderPass, NULL),
        material->pipeline);

    if (material->pipeline == NULL)
    {
        spDestroyRenderPass(material->renderPass, context->device);
        free(material);
        log_error(
            "Failed to make pipeline for material '%s'", createInfo->name);
        log_error(
            "Failed to make pipeline for material '%s'", createInfo->name);
        return NULL;
    }

    // set node buffer data
    material->currentBufferSpot = 0;
    for (u32 i = 0; i < array_length(material->nodeBuffer); i++)
    {
        material->nodeBuffer[i] = (struct t_SpiritMaterialListNode){
            .isBuffer = true, .used = false, .data = {}, .mesh = {}};
    }

    LIST_INIT(&material->meshList);

    return material;
}

SpiritResult
spMaterialUpdate(const SpiritContext context, SpiritMaterial material)
{

    return spRenderPassRecreateFramebuffers(
        context->device, material->renderPass, context->swapchain);
}

SpiritResult spMaterialAddMesh(
    const SpiritMaterial material,
    const SpiritMeshReference meshRef,
    SpiritPushConstant pushConstant)
{
    struct t_SpiritMaterialListNode *newNode = findNode(material);

    newNode->mesh         = spCheckoutMesh(meshRef);
    newNode->pushConstant = pushConstant;

    LIST_INSERT_HEAD(&material->meshList, newNode, data);
    material->meshCount++;
    return SPIRIT_SUCCESS;
}

SpiritResult spMaterialRecordCommands(
    const SpiritContext context, SpiritMaterial material, const u32 imageIndex)
{

    db_assert_msg(
        imageIndex < context->commandBufferCount, "invalid image index");

    SpiritCommandBuffer buf = context->commandBuffers[imageIndex];

    if (buf->state != SPIRIT_COMMAND_BUFFER_STATE_RECORDING)
    {
        log_error("Command buffer must be recording 🤓");
        return SPIRIT_FAILURE;
    }

    if (spRenderPassBegin(
            material->renderPass,
            context->screenResolution,
            imageIndex,
            context->commandBuffers[imageIndex]))
    {
        clearQueue(material);
        log_error("Failed to begin render pass");
        return SPIRIT_FAILURE;
    }

    if (spPipelineBindCommandBuffer(material->pipeline, buf))
    {
        log_error("Failed to bind command buffer");
        return SPIRIT_FAILURE;
    }

    // iterate through meshes and submit vertexes
    struct t_SpiritMaterialListNode *currentMesh =
        LIST_FIRST(&material->meshList);

    while (currentMesh != NULL)
    {

#ifndef FUNCTION_TIMER_NO_DIAGNOSTIC
        struct FunctionTimerData timer = start_timer("vertex commands");
#endif

        VkBuffer vertBuffers[] = {
            spMeshManagerAccessMesh(currentMesh->mesh)->vertexBuffer};
        VkDeviceSize offsets[] = {0};

        vkCmdBindVertexBuffers(buf->handle, 0, 1, vertBuffers, offsets);

        // push constants
        vkCmdPushConstants(
            buf->handle,
            material->pipeline->layout,
            VK_SHADER_STAGE_ALL_GRAPHICS,
            0,
            sizeof(SpiritPushConstant),
            &currentMesh->pushConstant);

        vkCmdDraw(buf->handle, currentMesh->mesh.vertCount, 1, 0, 0);

        // move to next list element
        struct t_SpiritMaterialListNode *oldNode = currentMesh;
        currentMesh                              = LIST_NEXT(currentMesh, data);

        spReleaseMesh(oldNode->mesh);
        LIST_REMOVE(oldNode, data);
        releaseNode(material, oldNode);

#ifndef FUNCTION_TIMER_NO_DIAGNOSTIC
        end_timer(timer);
#endif
    }

    time_function(spRenderPassEnd(context->commandBuffers[imageIndex]));

    // reset queue
    material->currentBufferSpot = 0;

    return SPIRIT_SUCCESS;
}

SpiritResult
spDestroyMaterial(const SpiritContext context, SpiritMaterial material)
{
    clearQueue(material);
    spDestroyPipeline(context->device, material->pipeline);
    spDestroyRenderPass(material->renderPass, context->device);
    free(material);
    return SPIRIT_SUCCESS;
}
