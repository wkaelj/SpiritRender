#include "spirit_context.h"

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

// 
// Public functions
// 

SpiritContext spCreateContext(SpiritContextCreateInfo *createInfo)
{

    SpiritContext context = new_var(struct t_SpiritContext);
    
    // initialize basic components
    // create window
    SpiritWindowCreateInfo windowCreateInfo;
    windowCreateInfo.windowSize = createInfo->windowSize;
    windowCreateInfo.title = createInfo->windowName;
    windowCreateInfo.fullscreen = createInfo->windowFullscreen;

    context->window = spCreateWindow (&windowCreateInfo);
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
        VK_KHR_SWAPCHAIN_EXTENSION_NAME,
    };

    deviceCreateInfo.requiredDeviceExtensions = deviceExtensions;
    deviceCreateInfo.requiredDeviceExtensionCount = 1;
    context->device = spCreateDevice (&deviceCreateInfo);
    db_assert(context->device, "Must have device");

    // create swapchain
    SpiritSwapchainCreateInfo swapCreateInfo = {};
    spWindowGetPixelSize(
        context->window, 
        &swapCreateInfo.windowWidthPx, 
        &swapCreateInfo.windowHeightPx);

    context->swapchain = spCreateSwapchain(swapCreateInfo, context->device, NULL);
    db_assert(context->swapchain, "Must have swapchain");

    context->commandBufferCount = context->swapchain->imageCount;
    context->commandBuffers = createCommandBuffers(
        context->device, 
        context->swapchain->imageCount);

    return context;
}

SpiritResult spContextSubmitFrame(SpiritContext context)
{
    // FIXME does not use linkedlist
    for (size_t i = 0; i < context->materialCount; i++)
    {
        spMaterialRecordCommands(context, context->materials[i]);
    }
    
    vkCmdDraw(context->commandBuffers[context->commandBufferIndex], 3, 0, 0, 0);
    return SPIRIT_SUCCESS;
}


SpiritResult spDestroyContext(SpiritContext context)
{

    destroyCommandBuffers(
        context->device, 
        context->commandBuffers, 
        context->commandBufferCount);
    
    for (u32 i = 0; i < context->materialCount; i++)
        spDestroyMaterial(context, context->materials[i]);
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
        dalloc(buffers);
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
    dalloc(buffers);
}

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
