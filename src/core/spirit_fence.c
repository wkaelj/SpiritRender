#include "spirit_fence.h"
#include "core/spirit_types.h"
#include "spirit_device.h"
#include "spirit_header.h"
#include <vulkan/vulkan_core.h>

SpiritFence spCreateFence(const SpiritDevice device, bool startSignaled)
{
    SpiritFence out = new_var(struct t_SpiritFence);
    out->isSignaled = startSignaled;

    VkFenceCreateInfo fenceInfo = {
        .sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO,
<<<<<<< HEAD
        .flags = startSignaled ? VK_FENCE_CREATE_SIGNALED_BIT : 0
    };

    if (vkCreateFence(
        device->device,
        &fenceInfo,
        ALLOCATION_CALLBACK,
        &out->handle)) return NULL;
=======
        .flags = startSignaled ? VK_FENCE_CREATE_SIGNALED_BIT : 0};

    if (vkCreateFence(
            device->device, &fenceInfo, ALLOCATION_CALLBACK, &out->handle))
        return NULL;
>>>>>>> devel

    return out;
}

<<<<<<< HEAD
SpiritResult spFenceWait(const SpiritDevice device, SpiritFence fence, u64 timeout_ns)
{

    if (fence->isSignaled) return SPIRIT_SUCCESS;
    else
    {
        VkResult r = vkWaitForFences(
            device->device,
            1,
            &fence->handle,
            VK_TRUE,
            timeout_ns);
        switch (r) {
        case VK_SUCCESS:
            fence->isSignaled = true;
            return SPIRIT_SUCCESS;
        case VK_TIMEOUT:
            log_warning("Timed out");
            break;
        case VK_ERROR_DEVICE_LOST:
            log_error("VK_ERROR_DEVICE_LOST.");
            break;
=======
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
>>>>>>> devel
        case VK_ERROR_OUT_OF_HOST_MEMORY:
            log_error("VK_ERROR_OUT_OF_HOST_MEMORY.");
            break;
        case VK_ERROR_OUT_OF_DEVICE_MEMORY:
            log_error("VK_ERROR_OUT_OF_DEVICE_MEMORY.");
            break;
<<<<<<< HEAD
        default:
            log_error("An unknown error has occurred.");
            break;
=======
        default: log_error("An unknown error has occurred."); break;
>>>>>>> devel
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

<<<<<<< HEAD

=======
>>>>>>> devel
void spDestroyFence(const SpiritDevice device, SpiritFence fence)
{

    if (fence->handle)
        vkDestroyFence(device->device, fence->handle, ALLOCATION_CALLBACK);

    free(fence);
}