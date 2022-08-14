/**
 * @file spirit_image.h
 * @author your name (you@domain.com)
 * @brief Handle the creation and destruction of images
 * @version 0.1
 * @date 2022-08-13
 *
 * @copyright Copyright (c) 2022
 *
 */

#pragma once

#include <spirit_header.h>

struct t_SpiritImage {
    VkImage image;
    VkDeviceMemory memory;
    VkFormat imageFormat;
    VkImageAspectFlags aspectFlags;
    SpiritResolution size;
    VkImageView view;
};

typedef struct t_SpiritImageCreateInfo {
    SpiritResolution size;
    VkImageCreateFlags flags;
    VkImageUsageFlags usageFlags;
    VkMemoryPropertyFlags memoryFlags;
    // TODO u32 memoryOffset;
    VkFormat format;
    u32 mipLevels;
    VkImageTiling tiling;
    bool withImageView; // if an image view should be created with the image
} SpiritImageCreateInfo;

// SpiritResult spCreateImage(
//     const SpiritDevice device,
//     VkImageType imageType,
//     );

/**
 * @brief add a image view to an existing image
 *
 * @param device the device the image was created with
 * @param output the SpiritImage to ad the image to
 */
SpiritResult spCreateImageView(
    const SpiritDevice device,
    SpiritImage *output) SPIRIT_NONULL(2);

/**
 * @brief Destroy a SpiritImage object, and its image view
 *
 * @param device the SpiritDevice the image was created using
 * @param image the image to destroy
 */
void spDestroyImage(const SpiritDevice device, SpiritImage *image) SPIRIT_NONULL(2);

/**
 * @brief Destroy a image view, without destroying the image
 *
 * @param device The device the image was created using
 * @param image
 */
void spDestroyImageView(const SpiritDevice device, SpiritImage *image) SPIRIT_NONULL(2);