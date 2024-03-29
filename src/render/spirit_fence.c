#include "spirit_fence.h"
#include "spirit_device.h"

SpiritFence spCreateFence(const SpiritDevice device, bool startSignaled)
{
    SpiritFence out = new_var(struct t_SpiritFence);
    out->isSignaled = startSignaled;

    VkFenceCreateInfo fenceInfo = {
        .sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO,
        .flags = startSignaled ? VK_FENCE_CREATE_SIGNALED_BIT : 0};

    if (vkCreateFence(
            device->device, &fenceInfo, ALLOCATION_CALLBACK, &out->handle))
        return NULL;

    return out;
}

SpiritResult
spFenceWait(const SpiritDevice device, SpiritFence fence, u64 timeout_ns)
{

    if (fence->isSignaled)
        return SPIRIT_SUCCESS;
    else
    {
        VkResult r = vkWaitForFences(
            device->device, 1, &fence->handle, VK_TRUE, timeout_ns);
        switch (r)
        {
        case VK_SUCCESS: fence->isSignaled = true; return SPIRIT_SUCCESS;
        case VK_TIMEOUT: log_warning("Timed out"); break;
        case VK_ERROR_DEVICE_LOST: log_error("VK_ERROR_DEVICE_LOST."); break;
        case VK_ERROR_OUT_OF_HOST_MEMORY:
            log_error("VK_ERROR_OUT_OF_HOST_MEMORY.");
            break;
        case VK_ERROR_OUT_OF_DEVICE_MEMORY:
            log_error("VK_ERROR_OUT_OF_DEVICE_MEMORY.");
            break;
        default: log_error("An unknown error has occurred."); break;
        }
    }

    return SPIRIT_FAILURE;
}

void spFenceReset(const SpiritDevice device, SpiritFence fence)
{
    if (fence->isSignaled)
    {
        fence->isSignaled = false;
        if (vkResetFences(device->device, 1, &fence->handle))
            log_warning("Failed to reset fence");
    }

    fence->isSignaled = false;
}

void spDestroyFence(const SpiritDevice device, SpiritFence fence)
{

    if (fence->handle)
        vkDestroyFence(device->device, fence->handle, ALLOCATION_CALLBACK);

    free(fence);
}