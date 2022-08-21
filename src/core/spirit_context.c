#include "spirit_context.h"

#include "spirit_command_buffer.h"
#include "spirit_device.h"
#include "spirit_swapchain.h"
#include "spirit_material.h"
#include "spirit_fence.h"

//
// Private functions
//

SpiritResult createSyncObjects(SpiritContext context);

void destroySyncObjects(SpiritContext context);

SpiritResult beginFrame(SpiritContext context, u32 *imageIndex);

SpiritResult endFrame(SpiritContext context, const u32 imageIndex);


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
    if (!context->window)
    {
        log_fatal("Must have window to create context");
        spDestroyContext(context);
        return NULL;
    }
    context->screenResolution = spWindowGetPixelSize(context->window);
    context->windowSize = createInfo->windowSize;
    db_assert(context->window);

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
    if (!context->device)
    {
        log_fatal("Must have device to create context");
        spDestroyContext(context);
        return NULL;
    }

    // create swapchain
    SpiritSwapchainCreateInfo swapCreateInfo = {};
    context->windowSize = spWindowGetPixelSize(context->window);
    swapCreateInfo.windowRes = context->screenResolution;

    swapCreateInfo.selectedPresentMode = true;
    swapCreateInfo.preferredPresentMode = VK_PRESENT_MODE_MAILBOX_KHR;

    context->swapchain = spCreateSwapchain(&swapCreateInfo, context->device, NULL);
    if (!context->swapchain)
    {
        log_fatal("Cannot create context without swapchain");
        spDestroyContext(context);
        return NULL;
    }

    // sync objects
    context->maxImagesInFlight = context->swapchain->imageCount - 1;
    if (createSyncObjects(context))
    {
        log_fatal("Failed to create sync objects");
        spDestroyContext(context);
        return NULL;
    }

    // create command buffers
    context->commandBufferCount = context->swapchain->imageCount;
    context->commandBuffers = new_array(
        SpiritCommandBuffer,
        context->commandBufferCount);

    for (u32 i = 0; i < context->commandBufferCount; ++i)
    {
        context->commandBuffers[i] = spCreateCommandBuffer(
            context->device, true);
        if (context->commandBuffers[i] == NULL)
        {
            log_fatal("Failed to create command buffer");
            spDestroyContext(context);
            return NULL;
        }
    }

    LIST_INIT(&context->materials);

    log_verbose("Created Context");

    return context;
}

SpiritResult spContextHandleWindowResized(SpiritContext context)
{
    if(!context && context->window) return SPIRIT_FAILURE;

    // recreate swapchain
    spDeviceWaitIdle(context->device);

    // update stored sizes
    context->screenResolution = spWindowGetPixelSize(context->window);
    context->windowSize = spWindowGetSize(context->window);

    // check swapchain exists, in case creation failed last frame
    SpiritSwapchainCreateInfo swapInfo = {};
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
        if (spContextHandleWindowResized(context))
        {
            log_warning("Failed to handle window resize");
            return SPIRIT_FAILURE;
        }
    }

    u32 imageIndex;

    if (beginFrame(context, &imageIndex))
    {
        log_fatal("Error beginning frame");
        return SPIRIT_FAILURE;
    }

    struct t_ContextMaterialListNode *np;
    LIST_FOREACH(np, &context->materials, data)
    {
        if (spMaterialRecordCommands(context, np->material, imageIndex))
        {
            log_error("Material %s failed to record commands", np->material->name);
        }
    }

    if (endFrame(context, imageIndex))
    {
        log_fatal("Error ending frame");
        return SPIRIT_FAILURE;
    }

    return SPIRIT_SUCCESS;
}

SpiritResult spContextAddMaterial(
    SpiritContext context,
    const SpiritMaterial material)
{

    db_assert(context);
    db_assert(material);

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

    ++context->materialCount;

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
            --context->materialCount;
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

    destroySyncObjects(context);

    for (u32 i = 0; context->commandBuffers && i < context->commandBufferCount; ++i)
    {
        if (context->commandBuffers[i])
            spDestroyCommandBuffer(context->device, context->commandBuffers[i]);
    }

    context->swapchain && spDestroySwapchain(context->swapchain, context->device);
    context->device && spDestroyDevice(context->device);
    context->window && spDestroyWindow (context->window);

    free(context);

    return SPIRIT_SUCCESS;
}

//
// Helpers
//

SpiritResult beginFrame(SpiritContext context, u32 *imageIndex)
{

    // if the imageInFlight fence is not null, wait for it to complete
    if (context->imagesInFlight[*imageIndex] && spFenceWait(
        context->device,
        context->inFlightFences[context->currentFrame],
        UINT64_MAX)) return SPIRIT_FAILURE;

    // aquire image
    if (spSwapchainAquireNextImage(
        context->device,
        context->swapchain,
        context->imageAvailableSemaphores[context->currentFrame],
        imageIndex)) return SPIRIT_FAILURE;

    SpiritCommandBuffer buf = context->commandBuffers[*imageIndex];

    spCommandBufferBegin(buf);

    // Dynamic state
    VkViewport viewport = {
        .x = 0.0f,
        .y = (f32) context->screenResolution.h,
        .width = (f32) context->screenResolution.w,
        .height = -(f32) context->screenResolution.h,
        .minDepth = 0.0f,
        .maxDepth = 1.0f
    };

    VkRect2D scissor = {
        .offset.x = scissor.offset.y = 0,
        .extent.width = context->screenResolution.w,
        .extent.height = context->screenResolution.h
    };

    vkCmdSetViewport(buf->handle, 0, 1, &viewport);
    vkCmdSetScissor(buf->handle, 0, 1, &scissor);

    return SPIRIT_SUCCESS;

}

SpiritResult endFrame(SpiritContext context, const u32 imageIndex)
{

    SpiritCommandBuffer buf = context->commandBuffers[imageIndex];
    const u32 currentFrame = context->currentFrame;

    spCommandBufferEnd(buf);

    // wait in case another frame was using this image
    if (context->imagesInFlight[imageIndex] && spFenceWait(
        context->device,
        context->imagesInFlight[imageIndex],
        UINT64_MAX)) return SPIRIT_FAILURE;

    context->imagesInFlight[imageIndex] = context->inFlightFences[currentFrame];

    spFenceReset(context->device, context->inFlightFences[currentFrame]);

    // submit command buffer
    if (spCommandBufferSubmit(
        context->device,
        buf,
        context->imageAvailableSemaphores[currentFrame],
        context->queueCompleteSemaphores[currentFrame],
        context->inFlightFences[currentFrame]))
    {
        log_fatal("Failed to submit command buffer");
        abort();
    }

    // present image
    spSwapchainPresent(
        context->device,
        context->swapchain,
        context->queueCompleteSemaphores[currentFrame],
        imageIndex);

    // increment current image
    context->currentFrame = (currentFrame + 1) % context->maxImagesInFlight;

    return SPIRIT_SUCCESS;
}

SpiritResult createSyncObjects(SpiritContext context)
{

    db_assert_msg(context->swapchain, "Context must have swapchain");

    const u32 imagesInFlight = context->maxImagesInFlight;
    db_assert_msg(imagesInFlight < 10, "Images in flight should not be more then 10")
    const u32 imageCount = context->swapchain->imageCount;
    db_assert_msg(imageCount < 11, "Image count should not be more then 10");
    db_assert_msg(imagesInFlight == imageCount - 1,
        "Images in flight should be imageCount - 1");

    context->imageAvailableSemaphores = new_array(VkSemaphore, imagesInFlight);
    context->queueCompleteSemaphores = new_array(VkSemaphore, imagesInFlight);
    context->inFlightFences = new_array(SpiritFence, imagesInFlight);


    VkSemaphoreCreateInfo semaphoreInfo = {
        .sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO,
        .flags = 0,
        .pNext = 0
    };

    for (size_t i = 0; i < imagesInFlight; i++) {

        bool failure = false;
        if (vkCreateSemaphore(
            context->device->device,
            &semaphoreInfo,
            NULL,
            &context->imageAvailableSemaphores[i])) failure = true;
       if (vkCreateSemaphore(
            context->device->device,
            &semaphoreInfo,
            NULL,
            &context->queueCompleteSemaphores[i])) failure = true;

        if (!(context->inFlightFences[i] = spCreateFence(context->device, true)))
        {
            failure = true;
        }

        if (failure)
        {
            log_error("Failed to create syncronization objects");
            return SPIRIT_FAILURE;
        }
    }

    context->imagesInFlight = new_array (SpiritFence, imageCount);

    // 0 the imagesInFlight memory, so that they are caught by the null
    // checks when waiting for them
    memset(context->imagesInFlight, 0, sizeof(SpiritFence) * imageCount);

    return SPIRIT_SUCCESS;
}

void destroySyncObjects(SpiritContext context)
{
    for (u32 i = 0; i < context->maxImagesInFlight; i++)
    {
        if (context->queueCompleteSemaphores && context->queueCompleteSemaphores[i])
            vkDestroySemaphore(context->device->device,
                context->queueCompleteSemaphores[i],
                ALLOCATION_CALLBACK);
        if (context->imageAvailableSemaphores && context->imageAvailableSemaphores[i])
            vkDestroySemaphore(context->device->device,
                context->imageAvailableSemaphores[i],
                NULL);
        if (context->inFlightFences && context->inFlightFences[i])
            spDestroyFence(context->device, context->inFlightFences[i]);
    }

    if (context->imageAvailableSemaphores) free(context->imageAvailableSemaphores);
    if (context->queueCompleteSemaphores)  free(context->queueCompleteSemaphores);
    if (context->inFlightFences)           free(context->inFlightFences);
    if (context->imagesInFlight)           free(context->imagesInFlight);
}
