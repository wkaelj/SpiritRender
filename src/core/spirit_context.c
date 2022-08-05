#include "spirit_context.h"

#include "spirit_device.h"
#include "spirit_swapchain.h"
#include "spirit_renderpass.h"
#include "spirit_pipeline.h"
#include "spirit_material.h"

// 
// Private functions
// 

VkCommandBuffer *createCommandBuffers(
    SpiritDevice device,
    u32          bufferCount);

void destroyCommandBuffers(
    const SpiritDevice device,
    VkCommandBuffer *buffers,
    const u32 bufferCount);

SpiritResult beginCommandBuffer(VkCommandBuffer buffer);

SpiritResult endCommandBuffer(VkCommandBuffer buffer);


// 
// Public functions
// 

SpiritContext spCreateContext(SpiritContextCreateInfo *createInfo)
{

    SpiritContext context = new_var(struct t_SpiritContext);
    
    // initialize basic components
    // create window
    SpiritWindowCreateInfo windowCreateInfo = {};
    windowCreateInfo.windowSize = createInfo->windowSize;
    windowCreateInfo.title = createInfo->windowName;
    windowCreateInfo.fullscreen = createInfo->windowFullscreen;

    context->window = spCreateWindow (&windowCreateInfo);
    context->screenResolution = spWindowGetPixelSize(context->window);
    context->windowSize = createInfo->windowSize;
    db_assert(context->window, "Must have window");

    // create device
    SpiritDeviceCreateInfo deviceCreateInfo = {};
    deviceCreateInfo.enableValidation = createInfo->enableValidation;
    deviceCreateInfo.powerSaveMode = createInfo->powerSaving;

    deviceCreateInfo.appName = "";
    deviceCreateInfo.appVersion = VK_MAKE_VERSION(0, 0, 0);
    deviceCreateInfo.engineName = "Spirit Render";
    deviceCreateInfo.engineVersion = VK_MAKE_VERSION(0, 0, 0);

    deviceCreateInfo.windowExtensions = spWindowGetExtensions(context->window);
    deviceCreateInfo.window = context->window;

    const char *deviceExtensions[1] = {
        VK_KHR_SWAPCHAIN_EXTENSION_NAME
    };

    deviceCreateInfo.requiredDeviceExtensions = deviceExtensions;
    deviceCreateInfo.requiredDeviceExtensionCount = 1;
    context->device = spCreateDevice (&deviceCreateInfo);
    db_assert(context->device, "Must have device");

    // create swapchain
    SpiritSwapchainCreateInfo swapCreateInfo = {};
    context->windowSize = spWindowGetPixelSize(context->window);
    swapCreateInfo.windowRes = context->screenResolution;

    context->swapchain = spCreateSwapchain(&swapCreateInfo, context->device, NULL);
    db_assert(context->swapchain, "Must have swapchain");

    context->commandBufferCount = context->swapchain->imageCount;
    context->commandBuffers = createCommandBuffers(
        context->device, 
        context->swapchain->imageCount);

    LIST_INIT(&context->materials);

    log_verbose("Created Context");

    return context;
}

SpiritResult spContextHandleWindowResized(SpiritContext context)
{
    if(!context && context->window) return SPIRIT_FAILURE;
    
    // recreate swapchain
    vkDeviceWaitIdle(context->device->device);

    // update stored sizes
    context->screenResolution = spWindowGetPixelSize(context->window);
    context->windowSize = spWindowGetSize(context->window);

    // check swapchain exists, in case creation failed last frame
    SpiritSwapchainCreateInfo swapInfo = {};
    if (context->swapchain) context->swapchain->createInfo;
    swapInfo.windowRes = context->screenResolution;

    context->swapchain = spCreateSwapchain(
        &swapInfo,
        context->device,
        context->swapchain);
    
    if (!context->swapchain)
    {
        return SPIRIT_FAILURE;
    }

    // iterate through materials
    struct t_ContextMaterialListNode *np;
    LIST_FOREACH(np, &context->materials, data)
    {
        spMaterialUpdate(context, np->material);
    }

    return SPIRIT_SUCCESS;

}

SpiritResult spContextSubmitFrame(SpiritContext context)
{

    // if failed to create swapchain last frame, try again now
    if (!context->swapchain)
    {
        spContextHandleWindowResized(context);
    }

    // aquire the image to render too
    if (spSwapchainAquireNextImage(
        context->device, 
        context->swapchain, 
        &context->commandBufferIndex)) return SPIRIT_FAILURE;

    if (beginCommandBuffer(context->commandBuffers[context->commandBufferIndex]))
    {
        log_fatal("Failed to begin command buffer %u", context->commandBufferIndex);
        abort();
    }
    context->isRecording = true;

    struct t_ContextMaterialListNode *np;
    LIST_FOREACH(np, &context->materials, data)
    {
        spMaterialRecordCommands(context, np->material);
    }

    context->isRecording = false;


    if (endCommandBuffer(context->commandBuffers[context->commandBufferIndex]))
    {
        log_fatal("Failed to end command buffer %u", context->commandBufferIndex);
        abort();
    }

    spSwapchainSubmitCommandBuffer(
        context->device,
        context->swapchain,
        context->commandBuffers[context->commandBufferIndex],
        context->commandBufferIndex);

    return SPIRIT_SUCCESS;
}

SpiritResult spContextAddMaterial(
    SpiritContext context,
    const SpiritMaterial material)
{

    db_assert(context, "Must have context");
    db_assert(material, "Must have material");

    if (!context && material)
    {
        return SPIRIT_FAILURE;
    }
    
    // ensure the same material is not added twice
    // possibly should be added to release builds
    #ifdef DEBUG
    struct t_ContextMaterialListNode *np;
    LIST_FOREACH(np, &context->materials, data)
    {
        if (np->material == material)
        {
            log_error("Same material added to context twice, address %p",
                material);
            return SPIRIT_FAILURE;
        }
    }
    #endif

    struct t_ContextMaterialListNode *node = 
        new_var(struct t_ContextMaterialListNode);

    node->material = material;
    
    LIST_INSERT_HEAD(&context->materials, node, data);

    return SPIRIT_SUCCESS;
}

SpiritResult spContextRemoveMaterial(
    SpiritContext context,
    const SpiritMaterial material)
{

    struct t_ContextMaterialListNode *np;
    LIST_FOREACH(np, &context->materials, data)
    {
        if (np->material == material)
        {
            LIST_REMOVE(np, data);
            free(np);
            return SPIRIT_SUCCESS;
        }
    }

    return SPIRIT_FAILURE;
}

SpiritResult spDestroyContext(SpiritContext context)
{

    // destroy materials
    struct t_ContextMaterialListNode *np, *op;
    np = LIST_FIRST(&context->materials);
    while(!LIST_EMPTY(&context->materials))
    {
        op = np;
        np = LIST_NEXT(np, data);
        LIST_REMOVE(op, data);
        free(op);
    }

    destroyCommandBuffers(
        context->device, 
        context->commandBuffers, 
        context->commandBufferCount);
    
    spDestroySwapchain(context->swapchain, context->device);
    spDestroyDevice(context->device);
    spDestroyWindow (context->window);

    return SPIRIT_SUCCESS;
}


//
// Command buffers
//

VkCommandBuffer *createCommandBuffers(
    SpiritDevice device,
    u32          bufferCount)
{

    VkCommandBuffer *buffers = new_array(VkCommandBuffer, bufferCount);

    VkCommandBufferAllocateInfo allocInfo = {};
    allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
    allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
    allocInfo.commandPool = device->commandPool;
    allocInfo.commandBufferCount = bufferCount;
    
    if (vkAllocateCommandBuffers(
        device->device, 
        &allocInfo, 
        buffers) != VK_SUCCESS)
    {
        log_error("Failed to allocate command buffers");
        free(buffers);
        return NULL;
    }

    return buffers;
}

void destroyCommandBuffers(
    const SpiritDevice device,
    VkCommandBuffer *buffers,
    const u32 bufferCount)
{

    vkFreeCommandBuffers(
        device->device, 
        device->commandPool, 
        bufferCount,
        buffers);
    free(buffers);
}

// command buffer creation and destruction
SpiritResult beginCommandBuffer(VkCommandBuffer buffer)
{

    VkCommandBufferBeginInfo bufferBeginInfo = {};
    bufferBeginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;

    if(vkBeginCommandBuffer(buffer, &bufferBeginInfo) != VK_SUCCESS)
    {
        log_error("Could not begin command buffer");
        return SPIRIT_FAILURE;
    }

    return SPIRIT_SUCCESS;
}

SpiritResult endCommandBuffer(VkCommandBuffer buffer)
{
    if (vkEndCommandBuffer(buffer)) return SPIRIT_FAILURE;
    return SPIRIT_SUCCESS;
}
