#include "spirit_material.h"

//
// Structures
//

// used to store a linkedlist of all mesh structs
struct t_SpiritMaterialMeshList
{
    SpiritMeshReference meshReference;
    struct t_SpiritMaterialMeshList *next;
};

//
// Helper functions
//
SpiritResult beginRenderPass(
    VkCommandBuffer buffer,
    const u32 index,
    const SpiritSwapchain swapchain, 
    const SpiritRenderPass renderPass);

void endRenderPass(VkCommandBuffer buffer);

static inline struct t_SpiritMaterialMeshList *newMeshList(const SpiritMeshReference ref);

// add a new mesh reference too the end of the list
static void meshListAdd(struct t_SpiritMaterialMeshList *meshList, const SpiritMeshReference);

// delete the mesh list and release the references
static void meshListDelete(struct t_SpiritMaterialMeshList *meshList);

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
    SpiritShader shaders[2];
    shaders[0].path = createInfo->fragmentShader;
    shaders[0].type = SPIRIT_SHADER_TYPE_FRAGMENT;
    shaders[1].path = createInfo->vertexShader;
    shaders[1].type = SPIRIT_SHADER_TYPE_VERTEX;
    pipelineCreateInfo.shaderFilePaths = shaders;
    pipelineCreateInfo.shaderFilePathCount = sizeof(shaders) / sizeof(shaders[0]);
    log_debug("material W=%u H=%u", pipelineCreateInfo.resolution.w, pipelineCreateInfo.resolution.w);

    pipelineCreateInfo.resolution = context->screenResolution;
    db_assert(pipelineCreateInfo.resolution.w == context->screenResolution.w &&
        pipelineCreateInfo.resolution.h == context->screenResolution.h, "wth");

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

    material->list = NULL;

    return material;

}

SpiritResult spMaterialAddMesh(
    const SpiritMaterial material,
    const SpiritMeshReference meshRef)
{
    meshListAdd(material->list, meshRef);

    return SPIRIT_SUCCESS;
}

size_t spMaterialRecordCommands(
    const SpiritContext context,
    SpiritMaterial material)
{
    db_assert(context->isRecording, "Context is not recording, cannot record commands");
    
    if(beginRenderPass(
        context->commandBuffers[context->commandBufferIndex],
        context->commandBufferIndex,
        context->swapchain,
        material->renderPass))
    {
        return SPIRIT_FAILURE;
    }

    // submit vertexes

    endRenderPass(context->commandBuffers[context->commandBufferIndex]);

    return SPIRIT_SUCCESS;

}

SpiritResult spDestroyMaterial(
    const SpiritContext context,
    SpiritMaterial material)
{
    spDestroyPipeline(context->device, material->pipeline);
    spDestroyRenderPass(material->renderPass, context->device);
    meshListDelete(material->list);
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

// list managment

inline struct t_SpiritMaterialMeshList *newMeshList(const SpiritMeshReference meshRef)
{
    // allocate space for a mesh component, check it out and add it too the list
    struct t_SpiritMaterialMeshList *meshList = new_var(struct t_SpiritMaterialMeshList);
    *meshList = (struct t_SpiritMaterialMeshList) {spCheckoutMesh(meshRef), NULL};
    return meshList;
}

void meshListAdd(struct t_SpiritMaterialMeshList *meshList, const SpiritMeshReference ref)
{

    struct t_SpiritMaterialMeshList *i = meshList;

    // find the last element in the mesh list 
    while(meshList->next != NULL) meshList = meshList->next;
    meshList->next = newMeshList(ref);
}

void meshListDelete(struct t_SpiritMaterialMeshList *meshList)
{
    
    struct t_SpiritMaterialMeshList *nextMesh;
    struct t_SpiritMaterialMeshList *currentMesh = meshList;

    while(currentMesh != NULL) 
    {
        nextMesh = currentMesh->next;

        // return mesh reference
        spReleaseMesh(currentMesh->meshReference);
        dalloc(currentMesh);

        currentMesh = nextMesh;
    }
}
