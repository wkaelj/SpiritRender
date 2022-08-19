#pragma once
#include <spirit_header.h>
#include <vulkan/vulkan_core.h>
#include "core/spirit_types.h"
#include "spirit_device.h"
#include "spirit_swapchain.h"
#include "spirit_renderpass.h"
#include "spirit_command_buffer.h"

// Render pass abtraction to create render passes based on templates, or advanced creation info
//
//
// Kael Johnston March 21 2022

//
// Types
//

// simple render pass templates, which can be used to
// create default render passes, like colour and shadow
// BE SMART:
// do not call SPIRIT_RENDER_PASS_MAX, it is to indicate
// the max enum value
typedef enum e_SpiritRenderPassType {
    SPIRIT_RENDER_PASS_TYPE_CUSTOM,
    SPIRIT_RENDER_PASS_TYPE_COLOUR,
    SPIRIT_RENDER_PASS_TYPE_SHADOW,
    SPIRIT_RENDER_PASS_TYPE_STENCIL,

    SPIRIT_RENDER_PASS_MAX
} SpiritRenderPassType;

// simplified render pass creation settings. can be converted to render pass create info using spRenderPassExpandSettings
typedef struct t_SpiritRenderPassSettings {

    SpiritRenderPassType type;
    const f32            colourFactors[3]; // multiply colour values by this

} SpiritRenderPassSettings;

// complex render pass create info
typedef struct t_SpiritRenderPassCreateInfo {

} SpiritRenderPassCreateInfo;

struct t_SpiritRenderPass
{
    VkRenderPass renderPass;
    VkFramebuffer *framebuffers;
    u32 framebufferCount;
};

//
// Functions
//

// create a render pass
SpiritRenderPass spCreateRenderPass(
    SpiritRenderPassCreateInfo *createInfo,
    const SpiritDevice          device,
    const SpiritSwapchain       swapchain);

/**
 * @brief Used to recreate the framebuffers for a renderpass.
 * It should be done if the window is resized, or the resolution is changed.
 *
 * @param renderPass
 * @param device
 * @return SpiritResult
 */
SpiritResult spRenderPassRecreateFramebuffers(
    const SpiritDevice device,
    SpiritRenderPass renderPass,
    const SpiritSwapchain swapchain);

/**
 * @brief Destroy a render pass
 *
 * @param renderPass
 * @param device
 * @return SpiritResult
 */
SpiritResult spDestroyRenderPass(
    SpiritRenderPass renderPass,
    SpiritDevice     device);

/**
 * @brief Convert render
 *
 */
__attribute__((unavailable)) SpiritRenderPassCreateInfo spRenderPassExpandSettings(
    SpiritRenderPassSettings *settings);

SpiritResult spRenderPassBegin(
    SpiritRenderPass renderPass,
    SpiritResolution framebufferSize,
    const u32 imageIndex,
    SpiritCommandBuffer commandBuffer);

SPIRIT_INLINE void spRenderPassEnd(SpiritCommandBuffer buffer)
{
    vkCmdEndRenderPass(buffer->handle);
}

