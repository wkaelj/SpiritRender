/**
 * @file spirit_command_buffer.h
 * @author your name (you@domain.com)
 * @brief Manage command buffers
 * @version 0.1
 * @date 2022-08-17
 *
 * @copyright Copyright (c) 2022
 *
 */

#pragma once

#include <spirit_header.h>

struct t_SpiritCommandBuffer {
    VkCommandBuffer handle;
    bool recording;
};

/**
 * @brief Create a command buffer
 *
 * @param device the device
 * @param primary whether or not the command buffer
 * should be a primary command buffer
 * @return SpiritCommandBuffer
 */
SpiritCommandBuffer spCreateCommandBuffer(SpiritDevice device, bool primary);

/**
 * @brief Create an array of command buffers
 *
 * @param device the device
 * @param buf must be an array of at least count
 * @param primary should they be primary command buffers
 * @param count how many command buffers
 * @return SpiritResult
 */
SpiritResult spCreateCommandBuffers(
    const SpiritDevice device,
    SpiritCommandBuffer *buf,
    bool primary,
    const u32 count) SPIRIT_NONULL(1, 2);

/**
 * @brief destroy a command buffer
 *
 * @param device
 * @param buffer
 */
void spDestroyCommandBuffer(const SpiritDevice device, SpiritCommandBuffer buffer);

/**
 * @brief begin a command buffer. allow it to recieve commands.
 *
 * @param buffer the command buffer to begin
 * @return SpiritResult
 */
SpiritResult spCommandBufferBegin(SpiritCommandBuffer buffer);

/**
 * @brief Submit a command buffer to the gpu
 *
 * @param device
 * @param buffer
 * @param waitSemaphore
 * @param signalSemaphore
 * @param fence
 * @return SpiritResult
 */
SpiritResult spCommandBufferSubmit(
    const SpiritDevice device,
    const SpiritCommandBuffer buffer,
    VkSemaphore waitSemaphore,
    VkSemaphore signalSemaphore,
    SpiritFence fence);

/**
 * @brief End a command buffer, so it can be submitted
 *
 * @param buffer
 * @return SpiritResult
 */
SpiritResult spCommandBufferEnd(SpiritCommandBuffer buffer);