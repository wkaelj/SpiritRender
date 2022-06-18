#pragma once
#include <spirit_header.h>
#include "spirit_device.h"
#include "spirit_renderpass.h"

// Creating and managment of pipelines
// multiple pipelines can share the same device
// 
// 
// Kael Johnston April 22 2022

//
// Types
// 

typedef struct t_SpiritPipelineCreateInfo {
    SpiritShader *shaderFilePaths;     // array of shader names
    u32           shaderFilePathCount; // num of shaders
    SpiritResolution resolution;
} SpiritPipelineCreateInfo;

struct t_SpiritPipeline
{
    VkPipeline    pipeline;
    VkPipelineLayout layout;    
    VkShaderModule *shaders;
    u32             shaderCount;
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
    VkCommandBuffer buffer);

// destroy a pipeline and free associated memory
SpiritResult spDestroyPipeline (
    SpiritDevice   device,
    SpiritPipeline pipeline);
