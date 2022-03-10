#pragma once

// public spirit functions and types
//
//
// Kael Johnston Mar 10 2022


//
// Render Pass
//

typedef enum e_SpiritRenderPassType {
    SPIRIT_RENDER_PASS_DEFAULT,
    SPIRIT_RENDER_PASS_TYPE_GRAPHICS,
    SPIRIT_RENDER_PASS_TYPE_STENCIL,

    SPIRIT_RENDER_PASS_MAX
} SpiritRenderPassType;

// simplified render pass creation settings. can be converted to render pass create info using spRenderPassExpandSettings
typedef struct t_SpiritRenderPassSettings {

    SpiritRenderPassType type;

} SpiritRenderPassSettings;

// complex render pass create info
typedef struct t_SpiritRenderPassCreateInfo {

} SpiritRenderPassCreateInfo;

// convert render pass settings into render pass create info
SpiritRenderPassCreateInfo spRenderPassExpandSettings (SpiritRenderPassSettings settings);
