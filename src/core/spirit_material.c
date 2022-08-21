#include "spirit_material.h"

#include "core/spirit_types.h"
#include "spirit_header.h"
#include "spirit_renderpass.h"
#include "spirit_pipeline.h"
#include "spirit_context.h"
#include "spirit_mesh.h"

#include "spirit_command_buffer.h"

//
// Structures
//

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

SpiritResult spMaterialRecordCommands(
    const SpiritContext context,
    SpiritMaterial material,
    const u32 imageIndex)
{

    db_assert_msg(imageIndex < context->commandBufferCount, "invalid image index");

    if (!context->commandBuffers[imageIndex]->recording)
    {
        log_error("Command buffer must be recording 🤓");
        return SPIRIT_FAILURE;
    }

    if(spRenderPassBegin(
        material->renderPass,
        context->screenResolution,
        imageIndex,
        context->commandBuffers[imageIndex]))
    {
        return SPIRIT_FAILURE;
    }



    spPipelineBindCommandBuffer(
        material->pipeline,
        context->commandBuffers[imageIndex]);

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
            context->commandBuffers[imageIndex]->handle,
            0,
            1,
            vertBuffers,
            offsets);

        vkCmdDraw(
            context->commandBuffers[imageIndex]->handle,
            currentMesh->mesh.vertCount,
            1, 0, 0);

        // move to next list element and remove processed element
        struct t_MaterialListNode *oldNode = currentMesh;
        currentMesh = LIST_NEXT(currentMesh, data);
        spReleaseMesh(oldNode->mesh);
        LIST_REMOVE(oldNode, data);
        free(oldNode);
    }

    spRenderPassEnd(context->commandBuffers[imageIndex]);

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
