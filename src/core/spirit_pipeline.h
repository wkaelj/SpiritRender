#pragma once
#include <spirit_header.h>

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

    u32 windowWidth, windowHeight; // window size
    u32 renderWidth, renderHeight; // rendering resolution

} SpiritPipelineCreateInfo;

// create a new render pass, can optionaly pass a old pipeline which will
// be recreated
SpiritPipeline spCreatePipeline (
    const SpiritDevice        device,
    SpiritPipelineCreateInfo *createInfo,
    const SpiritSwapchain     swapchain,
    // renderpasses?
    SpiritPipeline            optionalPipeline);

// destroy a pipeline and free associated memory
SpiritResult spDestroyPipeline (
    SpiritDevice   device,
    SpiritPipeline pipeline);