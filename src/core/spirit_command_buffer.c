#include "spirit_command_buffer.h"

<<<<<<< HEAD
#include "spirit_fence.h"
#include "spirit_device.h"
=======
#include "spirit_device.h"
#include "spirit_fence.h"
>>>>>>> devel

SpiritResult beginCommandBuffer(
    SpiritCommandBuffer buffer,
    bool singleUse,
    bool simultanious,
    bool renderpassContinue);

SpiritCommandBuffer spCreateCommandBuffer(SpiritDevice device, bool primary)
{

    SpiritCommandBuffer buffer = new_var(struct t_SpiritCommandBuffer);

    VkCommandBufferAllocateInfo allocInfo = {
<<<<<<< HEAD
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
=======
        .sType              = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO,
        .level              = primary ? VK_COMMAND_BUFFER_LEVEL_PRIMARY
                                      : VK_COMMAND_BUFFER_LEVEL_SECONDARY,
        .commandPool        = device->commandPool,
        .commandBufferCount = 1,
    };

    if (vkAllocateCommandBuffers(device->device, &allocInfo, &buffer->handle))
>>>>>>> devel
    {
        log_error("Failed to allocate command buffers");
        free(buffer);
        return NULL;
    }

    buffer->fence = spCreateFence(device, true);

    buffer->state = SPIRIT_COMMAND_BUFFER_STATE_READY;

    return buffer;
}

SpiritResult spCreateCommandBuffers(
    const SpiritDevice device,
    SpiritCommandBuffer *buf,
    bool primary,
    const u32 count)
{
    VkCommandBufferAllocateInfo allocInfo = {
<<<<<<< HEAD
        .sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO,
        .level = primary ?
            VK_COMMAND_BUFFER_LEVEL_PRIMARY :
            VK_COMMAND_BUFFER_LEVEL_SECONDARY,
        .commandPool = device->commandPool,
=======
        .sType              = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO,
        .level              = primary ? VK_COMMAND_BUFFER_LEVEL_PRIMARY
                                      : VK_COMMAND_BUFFER_LEVEL_SECONDARY,
        .commandPool        = device->commandPool,
>>>>>>> devel
        .commandBufferCount = count,
    };

    VkCommandBuffer buffer[count];
    if (vkAllocateCommandBuffers(device->device, &allocInfo, buffer))
    {
        return SPIRIT_FAILURE;
    }

<<<<<<< HEAD

    for (u32 i = 0; i < count; ++i)
    {
        buf[i]->handle = buffer[i];
        buf[i]->fence = spCreateFence(device, true);
        buf[i]->state = SPIRIT_COMMAND_BUFFER_STATE_READY;
=======
    for (u32 i = 0; i < count; ++i)
    {
        buf[i]->handle = buffer[i];
        buf[i]->fence  = spCreateFence(device, true);
        buf[i]->state  = SPIRIT_COMMAND_BUFFER_STATE_READY;
>>>>>>> devel
    }

    return SPIRIT_SUCCESS;
}

void spDestroyCommandBuffer(
<<<<<<< HEAD
    const SpiritDevice device,
    SpiritCommandBuffer buffer)
=======
    const SpiritDevice device, SpiritCommandBuffer buffer)
>>>>>>> devel
{

    // handle cases when the command buffer cannot be destroyed
    switch (buffer->state)
    {
    case SPIRIT_COMMAND_BUFFER_STATE_RECORDING:
        log_warning("Attempting to destroy a command buffer that is recording");
        spCommandBufferEnd(buffer);
        break;
    case SPIRIT_COMMAND_BUFFER_STATE_BUSY:
        log_warning("Attempting to destroy a command buffer that is busy");
        spCommandBufferWait(device, buffer, UINT64_MAX);
        break;
<<<<<<< HEAD
    default:
        break;
=======
    default: break;
>>>>>>> devel
    }

    spDestroyFence(device, buffer->fence);

    vkFreeCommandBuffers(
<<<<<<< HEAD
        device->device,
        device->commandPool,
        1,
        &buffer->handle);
=======
        device->device, device->commandPool, 1, &buffer->handle);
>>>>>>> devel

    free(buffer);
}

<<<<<<< HEAD
SpiritCommandBuffer spCreateCommandBufferAndBeginSingleUse(const SpiritDevice device)
=======
SpiritCommandBuffer
spCreateCommandBufferAndBeginSingleUse(const SpiritDevice device)
>>>>>>> devel
{
    SpiritCommandBuffer buf = spCreateCommandBuffer(device, true);
    spCommandBufferBeginSingleUse(buf);
    return buf;
}

SpiritResult spCommandBufferBeginSingleUse(SpiritCommandBuffer buf)
{
<<<<<<< HEAD
    if (buf->state != SPIRIT_COMMAND_BUFFER_STATE_READY) return SPIRIT_FAILURE;
=======
    if (buf->state != SPIRIT_COMMAND_BUFFER_STATE_READY)
        return SPIRIT_FAILURE;
>>>>>>> devel
    if (beginCommandBuffer(buf, true, false, false))
        return SPIRIT_FAILURE;

    buf->state = SPIRIT_COMMAND_BUFFER_STATE_RECORDING;

    return SPIRIT_SUCCESS;
}

// command buffer creation and destruction
SpiritResult spCommandBufferBegin(SpiritCommandBuffer buffer)
{

    if (buffer->state != SPIRIT_COMMAND_BUFFER_STATE_READY)
    {
<<<<<<< HEAD
        log_warning("Attemping to start command buffer that is not ready,"
            "use spCommandBufferWait() to wait for the command buffer.");

        return SPIRIT_FAILURE;

    }

    if(beginCommandBuffer(buffer, false, false, false)) return SPIRIT_FAILURE;
=======
        log_warning(
            "Attemping to start command buffer that is not ready,"
            "use spCommandBufferWait() to wait for the command buffer.");

        return SPIRIT_FAILURE;
    }

    if (beginCommandBuffer(buffer, false, false, false))
        return SPIRIT_FAILURE;
>>>>>>> devel

    buffer->state = SPIRIT_COMMAND_BUFFER_STATE_RECORDING;

    return SPIRIT_SUCCESS;
}

SpiritResult spCommandBufferSubmit(
    const SpiritDevice device,
    const SpiritCommandBuffer buffer,
    VkSemaphore waitSemaphore,
    VkSemaphore signalSemaphore,
    VkPipelineStageFlags *waitStages)
{

    if (buffer->state != SPIRIT_COMMAND_BUFFER_STATE_RECORDED)
    {
        log_warning("Must record and end buffer before submitting");
        return SPIRIT_FAILURE;
    }

    VkSubmitInfo submitInfo = {};
<<<<<<< HEAD
    submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;

    submitInfo.waitSemaphoreCount = waitSemaphore ? 1 : 0;
    submitInfo.pWaitSemaphores = &waitSemaphore;
    submitInfo.pWaitDstStageMask = waitStages;

    submitInfo.commandBufferCount = 1;
    submitInfo.pCommandBuffers = &buffer->handle;

    submitInfo.signalSemaphoreCount = signalSemaphore ? 1 : 0;
    submitInfo.pSignalSemaphores = &signalSemaphore;
=======
    submitInfo.sType        = VK_STRUCTURE_TYPE_SUBMIT_INFO;

    submitInfo.waitSemaphoreCount = waitSemaphore ? 1 : 0;
    submitInfo.pWaitSemaphores    = &waitSemaphore;
    submitInfo.pWaitDstStageMask  = waitStages;

    submitInfo.commandBufferCount = 1;
    submitInfo.pCommandBuffers    = &buffer->handle;

    submitInfo.signalSemaphoreCount = signalSemaphore ? 1 : 0;
    submitInfo.pSignalSemaphores    = &signalSemaphore;
>>>>>>> devel

    spFenceReset(device, buffer->fence);

    if (vkQueueSubmit(
<<<<<<< HEAD
        device->graphicsQueue,
        1,
        &submitInfo,
        buffer->fence->handle) != VK_SUCCESS)
=======
            device->graphicsQueue, 1, &submitInfo, buffer->fence->handle) !=
        VK_SUCCESS)
>>>>>>> devel
    {
        log_error("Failed to submit command buffer");
        return SPIRIT_FAILURE;
    }

    buffer->state = SPIRIT_COMMAND_BUFFER_STATE_BUSY;

    return SPIRIT_SUCCESS;
}

SpiritResult spCommandBufferSubmitSingleUse(
<<<<<<< HEAD
    const SpiritDevice device,
    SpiritCommandBuffer buffer)
{

    if (buffer->state == SPIRIT_COMMAND_BUFFER_STATE_RECORDING)
         spCommandBufferEnd(buffer);
    return spCommandBufferSubmit(device, buffer, NULL, NULL, NULL);

}

SpiritResult spCommandBufferWait(
    const SpiritDevice device,
    const SpiritCommandBuffer buffer,
    u64 timeout_ns)
=======
    const SpiritDevice device, SpiritCommandBuffer buffer)
{

    if (buffer->state == SPIRIT_COMMAND_BUFFER_STATE_RECORDING)
        spCommandBufferEnd(buffer);
    return spCommandBufferSubmit(device, buffer, NULL, NULL, NULL);
}

SpiritResult spCommandBufferWait(
    const SpiritDevice device, const SpiritCommandBuffer buffer, u64 timeout_ns)
>>>>>>> devel
{
    if (buffer->state != SPIRIT_COMMAND_BUFFER_STATE_BUSY)
    {
        switch (buffer->state)
        {
<<<<<<< HEAD
        case SPIRIT_COMMAND_BUFFER_STATE_READY:
            return SPIRIT_SUCCESS;
        case SPIRIT_COMMAND_BUFFER_STATE_RECORDED:
            log_warning("Waiting for command buffer that is recorded,"
                "you might need to submit the command buffer");
=======
        case SPIRIT_COMMAND_BUFFER_STATE_READY: return SPIRIT_SUCCESS;
        case SPIRIT_COMMAND_BUFFER_STATE_RECORDED:
            log_warning("Waiting for command buffer that is recorded,"
                        "you might need to submit the command buffer");
>>>>>>> devel
            break;
        case SPIRIT_COMMAND_BUFFER_STATE_RECORDING:
            log_error("You must end a command buffer before waiting for it");
            return SPIRIT_FAILURE;
<<<<<<< HEAD
        default:
            log_error("Unknown error waiting for command buffer");
=======
        default: log_error("Unknown error waiting for command buffer");
>>>>>>> devel
        }
        return SPIRIT_UNDEFINED;
    }

    if (spFenceWait(device, buffer->fence, timeout_ns))
    {
        return SPIRIT_FAILURE;
    }

    buffer->state = SPIRIT_COMMAND_BUFFER_STATE_READY;
    return SPIRIT_SUCCESS;
}

<<<<<<< HEAD

=======
>>>>>>> devel
SpiritResult spCommandBufferEnd(SpiritCommandBuffer buffer)
{
    if (buffer->state != SPIRIT_COMMAND_BUFFER_STATE_RECORDING)
    {
        log_warning("Trying to end command buffer that isn't recording");
        return SPIRIT_FAILURE;
    }
<<<<<<< HEAD
    if (vkEndCommandBuffer(buffer->handle)) return SPIRIT_FAILURE;
=======
    if (vkEndCommandBuffer(buffer->handle))
        return SPIRIT_FAILURE;
>>>>>>> devel
    buffer->state = SPIRIT_COMMAND_BUFFER_STATE_RECORDED;
    return SPIRIT_SUCCESS;
}

SpiritResult beginCommandBuffer(
    SpiritCommandBuffer buffer,
    bool singleUse,
    bool simultanious,
    bool renderpassContinue)
{

    VkCommandBufferBeginInfo bufferBeginInfo = {
        .sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO,
    };

    if (singleUse)
        bufferBeginInfo.flags |= VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;
    if (renderpassContinue)
<<<<<<< HEAD
        bufferBeginInfo.flags |= VK_COMMAND_BUFFER_USAGE_RENDER_PASS_CONTINUE_BIT;
    if (simultanious)
        bufferBeginInfo.flags |= VK_COMMAND_BUFFER_USAGE_SIMULTANEOUS_USE_BIT;

    if(vkBeginCommandBuffer(buffer->handle, &bufferBeginInfo))
=======
        bufferBeginInfo.flags |=
            VK_COMMAND_BUFFER_USAGE_RENDER_PASS_CONTINUE_BIT;
    if (simultanious)
        bufferBeginInfo.flags |= VK_COMMAND_BUFFER_USAGE_SIMULTANEOUS_USE_BIT;

    if (vkBeginCommandBuffer(buffer->handle, &bufferBeginInfo))
>>>>>>> devel
    {
        log_error("Could not begin command buffer");
        return SPIRIT_FAILURE;
    }

    return SPIRIT_SUCCESS;
}
