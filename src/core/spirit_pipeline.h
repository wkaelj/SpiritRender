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

} SpiritPipelineCreateInfo;

// create a new render pass, can optionaly pass a old pipeline which will
// be recreated
SpiritPipeline spCreatePipeline (
    SpiritPipelineCreateInfo *createInfo,
    SpiritDevice              device,
    SpiritSwapchain           swapchain,
    // renderpasses?
    SpiritPipeline            optionalPipeline);

// destroy a pipeline and free associated memory
SpiritResult spDestroyPipeline (
    SpiritPipeline pipeline, 
    SpiritDevice   device);