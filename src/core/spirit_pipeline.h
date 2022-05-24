#pragma once
#include <spirit_header.h>
#include "core/spirit_types.h"
#include "spirit_device.h"
#include "spirit_renderpass.h"
#include "spirit_mesh.h"

// Creating and managment of pipelines
// multiple pipelines can share the same device
//
//
// Kael Johnston April 22 2022

//
// Types
//

typedef struct t_SpiritPipelineCreateInfo {
    const char *vertexShader;     // array of shader names
    const char *fragmentShader; // num of shaders
    SpiritResolution resolution;
} SpiritPipelineCreateInfo;

struct t_SpiritPipeline
{
    VkPipeline pipeline;
    VkPipelineLayout layout;
};

// create a new render pass, can optionaly pass a old pipeline which will
// be recreated
SpiritPipeline spCreatePipeline (
    const SpiritDevice        device,
    SpiritPipelineCreateInfo *createInfo,
    const SpiritSwapchain     swapchain,
    const SpiritRenderPass    renderPass,
    SpiritPipeline            optionalPipeline);

SpiritResult spPipelineBindCommandBuffer(
    SpiritPipeline pipeline,
    SpiritCommandBuffer buffer);

// destroy a pipeline and free associated memory
SpiritResult spDestroyPipeline (
    SpiritDevice   device,
    SpiritPipeline pipeline);
