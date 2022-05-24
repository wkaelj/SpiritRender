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

typedef enum e_SpiritCommandBufferState {
    SPIRIT_COMMAND_BUFFER_STATE_READY = 0,
    SPIRIT_COMMAND_BUFFER_STATE_RECORDING,
    SPIRIT_COMMAND_BUFFER_STATE_RECORDED,
    SPIRIT_COMMAND_BUFFER_STATE_BUSY
} SpiritCommandBufferState;

struct t_SpiritCommandBuffer {
    VkCommandBuffer handle;
    SpiritFence fence;
    SpiritCommandBufferState state;
};

/**
 * @brief can be used to keep track of whether or not a command buffer is single
 * use.
 *
 */
typedef SpiritCommandBuffer SpiritCommandBufferSingleUse;

/**
 * @brief Create a command buffer
 *
 * @param device the device
 * @param primary whether or not the command buffer
 * should be a primary command buffer
 * @return SpiritCommandBuffer
 */
SpiritCommandBuffer spCreateCommandBuffer(
    const SpiritDevice device,
    bool primary);

/**
 * @brief create a command buffer that can only be used once
 *
 * @return SpiritCommandBuffer
 */
SpiritCommandBuffer spCreateCommandBufferSingleUse(const SpiritDevice);


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
 * @brief create a single use command buffer and begin its single usage
 *
 * @param device
 * @return SpiritResult
 */
SpiritCommandBuffer spCreateCommandBufferAndBeginSingleUse(
    const SpiritDevice device);

/**
 * @brief destroy a command buffer
 *
 * @param device
 * @param buffer
 */
void spDestroyCommandBuffer(
    const SpiritDevice device,
    SpiritCommandBuffer buffer);

/**
 * @brief begin a command buffer. allow it to recieve commands.
 *
 * @param buffer the command buffer to begin
 * @return SpiritResult
 */
SpiritResult spCommandBufferBegin(SpiritCommandBuffer buffer);

/**
 * @brief begin a command buffer for single use commands
 *
 * @param buf
 * @return SpiritResult
 */
SpiritResult spCommandBufferBeginSingleUse(SpiritCommandBuffer buf);

/**
 * @brief Submit a command buffer to the gpu
 *
 * @param device
 * @param buffer the command buffer
 * @param waitSemaphore the wait semaphore, if any
 * @param signalSemaphore the signal semaphore, if any
 * @param waitStages the pipeline stages to wait for, if any
 * @param fence the fence to use to wait
 * @return SpiritResult
 */
SpiritResult spCommandBufferSubmit(
    const SpiritDevice device,
    const SpiritCommandBuffer buffer,
    VkSemaphore waitSemaphore,
    VkSemaphore signalSemaphore,
    VkPipelineStageFlags *waitStages) SPIRIT_NONULL(1, 2);

/**
 * @brief Wait for a command buffer to finish executiona
 *
 * @param device
 * @param buffer
 * @param timeout_ns
 * @return SpiritResult
 */
SpiritResult spCommandBufferWait(
    const SpiritDevice device,
    const SpiritCommandBuffer buffer,
    u64 timeout_ns) SPIRIT_NONULL(1, 2);
/**
 * @brief submit the single use of a singl use command buffer. The command buffer
 * must be destroyed immidiatly after
 *
 * @param device
 * @param buffer
 * @return SpiritResult
 */
SpiritResult spCommandBufferSubmitSingleUse(
    const SpiritDevice device,
    SpiritCommandBuffer buffer) SPIRIT_NONULL(1, 2);

/**
 * @brief End a command buffer, so it can be submitted
 *
 * @param buffer
 * @return SpiritResult
 */
SpiritResult spCommandBufferEnd(SpiritCommandBuffer buffer);

SPIRIT_INLINE SpiritCommandBufferState spCommandBufferGetState(
    const SpiritCommandBuffer buf)
{
    return buf->state;
}