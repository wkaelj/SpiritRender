#include "spirit_command_buffer.h"

#include "core/spirit_types.h"
#include "debug/messenger.h"
#include "spirit_device.h"
#include "spirit_header.h"
#include <vulkan/vulkan_core.h>

SpiritCommandBuffer spCreateCommandBuffer(SpiritDevice device, bool primary)
{

    SpiritCommandBuffer buffer = new_var(struct t_SpiritCommandBuffer);

    VkCommandBufferAllocateInfo allocInfo = {
        .sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO,
        .level = primary ?
            VK_COMMAND_BUFFER_LEVEL_PRIMARY :
            VK_COMMAND_BUFFER_LEVEL_SECONDARY,
        .commandPool = device->commandPool,
        .commandBufferCount = 1
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
