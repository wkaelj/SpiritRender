/**
 * @file spirit_sync.h
 * @author kale name (you@domain.com)
 * @brief Manage gpu syncronization
 * @version 0.1
 * @date 2022-08-15
 *
 * @copyright Copyright (c) 2022
 *
 */

// TODO better docs

#pragma once

#include <spirit_header.h>

<<<<<<< HEAD
struct t_SpiritFence {
=======
struct t_SpiritFence
{
>>>>>>> devel
    VkFence handle;
    bool isSignaled;
};

/**
 * @brief Create a SpiritFence
 *
 * @param device
 * @param startSignaled whether the fence should be created signaled
 * @return SpiritFence
 */
<<<<<<< HEAD
SpiritFence spCreateFence(const SpiritDevice device, bool startSignaled) SPIRIT_NONULL(1);
=======
SpiritFence spCreateFence(const SpiritDevice device, bool startSignaled)
    SPIRIT_NONULL(1);
>>>>>>> devel

/**
 * @brief Wait for a fence to complete. This function blocks
 *
 * @param fence the fence to wait for
 * @return SpiritResult
 */
<<<<<<< HEAD
SpiritResult spFenceWait(
    const SpiritDevice device,
    SpiritFence fence,
    u64 timeout_ns) SPIRIT_NONULL(1, 2);
=======
SpiritResult
spFenceWait(const SpiritDevice device, SpiritFence fence, u64 timeout_ns)
    SPIRIT_NONULL(1, 2);
>>>>>>> devel

/**
 * @brief Reset a fence
 *
 * @param fence
 */
<<<<<<< HEAD
void spFenceReset(const SpiritDevice device, SpiritFence fence) SPIRIT_NONULL(1, 2);
=======
void spFenceReset(const SpiritDevice device, SpiritFence fence)
    SPIRIT_NONULL(1, 2);
>>>>>>> devel

/**
 * @brief destroy a spirit fence
 *
 * @param device
 * @param fence
 */
<<<<<<< HEAD
void spDestroyFence(const SpiritDevice device, SpiritFence fence) SPIRIT_NONULL(1, 2);
=======
void spDestroyFence(const SpiritDevice device, SpiritFence fence)
    SPIRIT_NONULL(1, 2);
>>>>>>> devel
