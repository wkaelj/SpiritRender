#pragma once
#include <spirit_header.h>
#include "spirit_device.h"
#include "spirit_swapchain.h"
#include "spirit_renderpass.h"

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
};

//
// Functions
//

// create a render pass
SpiritRenderPass spCreateRenderPass(
    SpiritRenderPassCreateInfo *createInfo, 
    const SpiritDevice          device,
    const SpiritSwapchain       swapchain);

SpiritResult spDestroyRenderPass(
    SpiritRenderPass renderPass, 
    SpiritDevice     device);

// convert render pass settings into render pass create info
SpiritRenderPassCreateInfo spRenderPassExpandSettings (
    SpiritRenderPassSettings *settings);
