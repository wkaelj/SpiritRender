#include "spirit_context.h"

// 
// Private functions
// 

// submit a draw call
void submitDrawCall(
    SpiritDevice device,
    SpiritRenderPass renderPass,
    SpiritSwapchain swapchain,
    SpiritPipeline pipeline,
    VkCommandBuffer *buffers,
    const u32 bufferCount);

VkCommandBuffer *createCommandBuffers(
    SpiritDevice device,
    VkRenderPass renderPass,
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
    windowCreateInfo.w = createInfo->windowSize.w;
    windowCreateInfo.h = createInfo->windowSize.w;
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

    SpiritRenderPassCreateInfo renderPassCreateInfo = {};

    context->renderPasses = new_array(SpiritRenderPass, 1);
    context->renderPassCount = 1;
    context->renderPasses[0] = spCreateRenderPass(&renderPassCreateInfo, context->device, context->swapchain);

    SpiritPipelineCreateInfo pipelineCreateInfo = {};
    pipelineCreateInfo.shaderFilePathCount = 2;
    SpiritShader shaders[2];
    shaders[0].path = createInfo->fragmentShader;
    shaders[0].type = SPIRIT_SHADER_TYPE_FRAGMENT;
    shaders[1].path = createInfo->vertexShader;
    shaders[1].type = SPIRIT_SHADER_TYPE_VERTEX;
    pipelineCreateInfo.shaderFilePaths = shaders;
    pipelineCreateInfo.windowWidth = swapCreateInfo.windowWidthPx;
    pipelineCreateInfo.windowHeight = swapCreateInfo.windowHeightPx;
    pipelineCreateInfo.renderWidth = swapCreateInfo.windowWidthPx;
    pipelineCreateInfo.renderHeight = swapCreateInfo.windowHeightPx;

    context->pipelines = new_array(SpiritPipeline, 1);
    context->pipelines[0] = spCreatePipeline (
        context->device, 
        &pipelineCreateInfo,
        context->swapchain,
        context->renderPasses[0], 
        NULL);
    
    context->commandBufferCount = context->swapchain->imageCount;
    context->commandBuffers = createCommandBuffers(
        context->device, 
        context->renderPasses[0]->renderPass,
        context->swapchain->imageCount);

    return context;
}

SpiritResult spContextSubmitFrame(SpiritContext context)
{
    submitDrawCall(
        context->device, 
        context->renderPasses[0], 
        context->swapchain, 
        context->pipelines[0], 
        context->commandBuffers, 
        context->commandBufferCount);
    return SPIRIT_SUCCESS;
}


SpiritResult spDestroyContext(SpiritContext context)
{

    destroyCommandBuffers(
        context->device, 
        context->commandBuffers, 
        context->commandBufferCount);
    
    spDestroyPipeline (context->device, context->pipelines[0]);

    for (u32 i = 0; i < context->renderPassCount; i++)
        spDestroyRenderPass(context->renderPasses[i], context->device);
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
    VkRenderPass renderPass,
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

void submitDrawCall(
    SpiritDevice device,
    SpiritRenderPass renderPass,
    SpiritSwapchain swapchain,
    SpiritPipeline pipeline,
    VkCommandBuffer *buffers,
    const u32 bufferCount)
{

    u32 swapchainImageIndex;
    spSwapchainAquireNextImage(device, swapchain, &swapchainImageIndex);
    beginCommandBuffer(buffers[swapchainImageIndex], pipeline);
    beginRenderPass(
        buffers[swapchainImageIndex],
        swapchainImageIndex,
        swapchain,
        renderPass);

    vkCmdDraw(buffers[swapchainImageIndex], 3, 1, 0, 0);

    endRenderPass(buffers[swapchainImageIndex]);
    endCommandBuffer(buffers[swapchainImageIndex]);

    spSwapchainSubmitCommandBuffer(
        device,
        swapchain,
        buffers[swapchainImageIndex],
        swapchainImageIndex);
        
}

