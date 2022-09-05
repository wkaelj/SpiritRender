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
<<<<<<< HEAD
#include <vulkan/vulkan_core.h>

struct t_SpiritImage {
=======

struct t_SpiritImage
{
>>>>>>> devel
    VkImage image;
    VkDeviceMemory memory;
    VkFormat imageFormat;
    VkImageAspectFlags aspectFlags;
    SpiritResolution size;
    VkImageView view;
};

<<<<<<< HEAD
typedef struct t_SpiritImageCreateInfo {
=======
typedef struct t_SpiritImageCreateInfo
{
>>>>>>> devel
    SpiritResolution size;
    VkImageCreateFlags flags;
    VkImageUsageFlags usageFlags;
    VkMemoryPropertyFlags memoryFlags;
    VkImageAspectFlags aspectFlags;
    // TODO u32 memoryOffset;
    VkFormat format;
    u32 mipLevels;
    VkImageTiling tiling;
    bool withImageView; // if an image view should be created with the image
} SpiritImageCreateInfo;

/**
 * @brief Create a image on the gpu
 *
 * @param device the the device to create the image on
 * @param createInfo a SpiritImageCreateInfo struct detailing the image
 * @param output the handle to the image
 */
SpiritResult spCreateImage(
    const SpiritDevice device,
    SpiritImageCreateInfo *restrict createInfo,
    SpiritImage *restrict output) SPIRIT_NONULL(2, 3);

/**
 * @brief add a image view to an existing image
 *
 * @param device the device the image was created with
 * @param output the SpiritImage to ad the image to
 */
<<<<<<< HEAD
SpiritResult spCreateImageView(
    const SpiritDevice device,
    SpiritImage *output) SPIRIT_NONULL(2);
=======
SpiritResult spCreateImageView(const SpiritDevice device, SpiritImage *output)
    SPIRIT_NONULL(2);
>>>>>>> devel

/**
 * @brief get the format of a spirit image
 *
 * @param image
 * @return VkFormat
 */
VkFormat spImageGetFormat(const SpiritImage *image);

/**
 * @brief Access an images imageView. Will be null if there is no image view
 *
 * @param image
 * @return VkImageView
 */
VkImageView spImageGetVkView(const SpiritImage *image);

/**
 * @brief Destroy a SpiritImage object, and its image view
 *
 * @param device the SpiritDevice the image was created using
 * @param image the image to destroy
 */
<<<<<<< HEAD
void spDestroyImage(const SpiritDevice device, SpiritImage *image) SPIRIT_NONULL(2);
=======
void spDestroyImage(const SpiritDevice device, SpiritImage *image)
    SPIRIT_NONULL(2);
>>>>>>> devel

/**
 * @brief Destroy a image view, without destroying the image
 *
 * @param device The device the image was created using
 * @param image
 */
<<<<<<< HEAD
void spDestroyImageView(const SpiritDevice device, SpiritImage *image) SPIRIT_NONULL(2);
=======
void spDestroyImageView(const SpiritDevice device, SpiritImage *image)
    SPIRIT_NONULL(2);
>>>>>>> devel
