#include "spirit_material.h"

#include "spirit_context.h"
#include "spirit_mesh.h"
#include "spirit_pipeline.h"
#include "spirit_renderpass.h"

#include "spirit_command_buffer.h"

//
// Helper functions
//

SPIRIT_INLINE void clearQueue(SpiritMaterial material)
{

    while (!LIST_EMPTY(&material->meshList))
        LIST_REMOVE(LIST_FIRST(&material->meshList), data);
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

    pipelineCreateInfo.resolution = context->screenResolution;

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
        return NULL;
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

SpiritResult
spMaterialAddMesh(SpiritMaterial material, const SpiritMeshReference meshRef)
{
    struct t_MaterialListNode *newNode = new_var(struct t_MaterialListNode);
    newNode->mesh                      = spCheckoutMesh(meshRef);

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
        clearQueue(material);
        return SPIRIT_FAILURE;
    }

    SpiritResult result;
    time_function_with_return(
        spRenderPassBegin(
            material->renderPass,
            context->screenResolution,
            imageIndex,
            context->commandBuffers[imageIndex]),
        result);
    if (result)
    {
        clearQueue(material);
        log_error("Failed to begin render pass");
        return SPIRIT_FAILURE;
    }

    time_function_with_return(
        spPipelineBindCommandBuffer(material->pipeline, buf), result);

    if (result)
    {
        log_error("Failed to bind command buffer");
        return SPIRIT_FAILURE;
    }

    // iterate through meshes and submit vertexes
    struct t_MaterialListNode *currentMesh = LIST_FIRST(&material->meshList);

    while (currentMesh != NULL)
    {

#ifndef FUNCTION_TIMER_NO_DIAGNOSTIC
        struct FunctionTimerData timer = start_timer("vertex commands");
#endif

        VkBuffer vertBuffers[] = {
            spMeshManagerAccessMesh(currentMesh->mesh)->vertexBuffer};
        VkDeviceSize offsets[] = {0};

        vkCmdBindVertexBuffers(buf->handle, 0, 1, vertBuffers, offsets);

        vkCmdDraw(buf->handle, currentMesh->mesh.vertCount, 1, 0, 0);

        // move to next list element and remove processed element
        struct t_MaterialListNode *oldNode = currentMesh;
        currentMesh                        = LIST_NEXT(currentMesh, data);
        spReleaseMesh(oldNode->mesh);
        LIST_REMOVE(oldNode, data);
        free(oldNode);

#ifndef FUNCTION_TIMER_NO_DIAGNOSTIC
        end_timer(timer);
#endif
    }

    time_function(spRenderPassEnd(context->commandBuffers[imageIndex]));

    return SPIRIT_SUCCESS;
}

SpiritResult
spDestroyMaterial(const SpiritContext context, SpiritMaterial material)
{
    spDestroyPipeline(context->device, material->pipeline);
    spDestroyRenderPass(material->renderPass, context->device);
    free(material);
    return SPIRIT_SUCCESS;
}
