#include "spirit_command_buffer.h"

#include "spirit_fence.h"
#include "spirit_device.h"

SpiritCommandBuffer spCreateCommandBuffer(SpiritDevice device, bool primary)
{

    SpiritCommandBuffer buffer = new_var(struct t_SpiritCommandBuffer);

    VkCommandBufferAllocateInfo allocInfo = {
        .sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO,
        .level = primary ?
            VK_COMMAND_BUFFER_LEVEL_PRIMARY :
            VK_COMMAND_BUFFER_LEVEL_SECONDARY,
        .commandPool = device->commandPool,
        .commandBufferCount = 1,
    };

    if (vkAllocateCommandBuffers(
        device->device,
        &allocInfo,
        &buffer->handle))
    {
        log_error("Failed to allocate command buffers");
        free(buffer);
        return NULL;
    }

    buffer->recording = false;

    return buffer;
}

SpiritResult spCreateCommandBuffers(
    const SpiritDevice device,
    SpiritCommandBuffer *buf,
    bool primary,
    const u32 count)
{
    VkCommandBufferAllocateInfo allocInfo = {
        .sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO,
        .level = primary ?
            VK_COMMAND_BUFFER_LEVEL_PRIMARY :
            VK_COMMAND_BUFFER_LEVEL_SECONDARY,
        .commandPool = device->commandPool,
        .commandBufferCount = count,
    };

    VkCommandBuffer buffer[count];
    if (vkAllocateCommandBuffers(device->device, &allocInfo, buffer))
    {
        return SPIRIT_FAILURE;
    }

    for (u32 i = 0; i < count; ++i)
    {
        buf[i]->handle = buffer[i];
        buf[i]->recording = false;
    }

    return SPIRIT_SUCCESS;
}

void spDestroyCommandBuffer(
    const SpiritDevice device,
    SpiritCommandBuffer buffer)
{

    vkFreeCommandBuffers(
        device->device,
        device->commandPool,
        1,
        &buffer->handle);
    free(buffer);
}

// command buffer creation and destruction
SpiritResult spCommandBufferBegin(SpiritCommandBuffer buffer)
{

    if (buffer->recording)
    {
        log_warning("Attemping to start command buffer that is already recording");
        return SPIRIT_FAILURE;
    }

    static const VkCommandBufferBeginInfo bufferBeginInfo = {
        .sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO
    };

    if(vkBeginCommandBuffer(buffer->handle, &bufferBeginInfo))
    {
        log_error("Could not begin command buffer");
        return SPIRIT_FAILURE;
    }

    buffer->recording = true;

    return SPIRIT_SUCCESS;
}

SpiritResult spCommandBufferSubmit(
    const SpiritDevice device,
    const SpiritCommandBuffer buffer,
    VkSemaphore waitSemaphore,
    VkSemaphore signalSemaphore,
    SpiritFence fence)
{

    if (buffer->recording)
    {
        log_warning("Must end buffer before submitting");
        return SPIRIT_FAILURE;
    }

    VkSubmitInfo submitInfo = {};
    submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;

    VkPipelineStageFlags waitStages[] = {
        VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT
    };

    submitInfo.waitSemaphoreCount = 1;
    submitInfo.pWaitSemaphores = &waitSemaphore;
    submitInfo.pWaitDstStageMask = waitStages;

    submitInfo.commandBufferCount = 1;
    submitInfo.pCommandBuffers = &buffer->handle;

    submitInfo.signalSemaphoreCount = 1;
    submitInfo.pSignalSemaphores = &signalSemaphore;

    if (vkQueueSubmit(
        device->graphicsQueue,
        1,
        &submitInfo,
        fence->handle) != VK_SUCCESS)
    {
        log_error("Failed to submit graphics queue");
        return SPIRIT_FAILURE;
    }

    return SPIRIT_SUCCESS;
}

SpiritResult spCommandBufferEnd(SpiritCommandBuffer buffer)
{
    if (!buffer->recording)
    {
        log_warning("Trying to end command buffer that isn't recording");
        return SPIRIT_FAILURE;
    }
    if (vkEndCommandBuffer(buffer->handle)) return SPIRIT_FAILURE;
    buffer->recording = false;
    return SPIRIT_SUCCESS;
}
