#include "spirit_image.h"
#include "spirit_device.h"

SpiritResult spCreateImage(
    const SpiritDevice device,
    SpiritImageCreateInfo *restrict createInfo,
    SpiritImage *restrict output)
{
    VkImageCreateInfo imageInfo = {
        .sType         = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO,
        .flags         = createInfo->flags,
        .imageType     = VK_IMAGE_TYPE_2D,
        .format        = createInfo->format,
        .extent        = {createInfo->size.w, createInfo->size.h, 1},
        .mipLevels     = createInfo->mipLevels,
        .arrayLayers   = 1,
        .samples       = VK_SAMPLE_COUNT_1_BIT,
        .tiling        = VK_IMAGE_TILING_OPTIMAL,
        .usage         = createInfo->usageFlags,
        .sharingMode   = VK_SHARING_MODE_EXCLUSIVE,
        .initialLayout = VK_IMAGE_LAYOUT_UNDEFINED
    };

    if (vkCreateImage(
            device->device, &imageInfo, ALLOCATION_CALLBACK, &output->image))
    {
        return SPIRIT_FAILURE;
    }

    VkMemoryRequirements memoryRequirements;
    vkGetImageMemoryRequirements(
        device->device, output->image, &memoryRequirements);

    u32 memoryType = spDeviceFindMemoryType(
        device, memoryRequirements.memoryTypeBits, createInfo->memoryFlags);
    if (memoryType == (unsigned)-1)
    {
        spDestroyImage(device, output);
        return SPIRIT_FAILURE;
    }

    if (spDeviceAllocateMemory(
            device, memoryRequirements.size, memoryType, &output->memory))
    {
        spDestroyImage(device, output);
        return SPIRIT_FAILURE;
    }

    if (vkBindImageMemory(
            device->device,
            output->image,
            output->memory,
            0 /* createInfo->memoryOffset */))
    {
        spDestroyImage(device, output);
        return SPIRIT_FAILURE;
    }

    output->imageFormat = createInfo->format;
    output->aspectFlags = createInfo->aspectFlags;

    output->view = NULL;

    if (createInfo->withImageView)
        spCreateImageView(device, output);

    return SPIRIT_SUCCESS;
}

SpiritResult spCreateImageView(const SpiritDevice device, SpiritImage *out)
{

    if (out->image == NULL)
        return SPIRIT_FAILURE;

    if (out->view)
    {
        log_warning(
            "Image view already exists. The image view will be recreated");
        spDestroyImageView(device, out);
    }

    VkImageViewCreateInfo viewInfo = {
        .sType            = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO,
        .flags            = 0,
        .image            = out->image,
        .viewType         = VK_IMAGE_VIEW_TYPE_2D,
        .format           = out->imageFormat,
        .subresourceRange = {
                             .baseMipLevel   = 0,
                             .levelCount     = 1,
                             .baseArrayLayer = 0,
                             .layerCount     = 1,
                             .aspectMask     = out->aspectFlags}
    };

    if (vkCreateImageView(
            device->device, &viewInfo, ALLOCATION_CALLBACK, &out->view))
        return SPIRIT_FAILURE;

    return SPIRIT_SUCCESS;
}

SPIRIT_INLINE VkFormat spImageGetFormat(const SpiritImage *image)
{
    return image->imageFormat;
}

SPIRIT_INLINE VkImageView spImageGetVkView(const SpiritImage *image)
{
    return image->view;
}

void spDestroyImage(const SpiritDevice device, SpiritImage *image)
{
    if (image->view)
        vkDestroyImageView(device->device, image->view, ALLOCATION_CALLBACK);
    image->view = NULL;
    if (image->memory)
        spDeviceFreeMemory(device, image->memory);
    image->memory = NULL;
    if (image->image)
        vkDestroyImage(device->device, image->image, ALLOCATION_CALLBACK);
    image->image = NULL;
}

void spDestroyImageView(const SpiritDevice device, SpiritImage *image)
{
    if (image->view)
        vkDestroyImageView(device->device, image->view, ALLOCATION_CALLBACK);
    image->view = NULL;
}