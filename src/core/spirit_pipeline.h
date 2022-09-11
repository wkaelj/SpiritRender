/**
 * @file spirit_pipeline.h
 * @author Kael Johnston (wkaelj@gmail.com)
 * @brief Create rendering pipelines
 * @version 0.2
 * @date 2022-08-28
 *
 * @copyright Copyright (c) 2022
 *
 */

#pragma once
#include <spirit_header.h>
#include "core/spirit_types.h"
#include "spirit_device.h"
#include "spirit_renderpass.h"
#include "spirit_mesh.h"

//
// Types
//

/**
 * @brief Informatino to create a pipeline.
 *
 */
typedef struct t_SpiritPipelineCreateInfo
{
    const char *vertexShader;   // array of shader names
    const char *fragmentShader; // num of shaders
    SpiritResolution resolution;
} SpiritPipelineCreateInfo;

struct t_SpiritPipeline
{
    VkPipeline pipeline;
    VkPipelineLayout layout;
};

/**
 * @brief Create a new pipeline
 *
 * @param device
 * @param createInfo
 * @param renderPass
 * @param optionalPipeline
 * @return SpiritPipeline
 */
SpiritPipeline spCreatePipeline(
    const SpiritDevice device,
    SpiritPipelineCreateInfo *createInfo,
    const SpiritRenderPass renderPass,
    SpiritPipeline optionalPipeline);

/**
 * @brief Bind a pipeline to a command buffer
 *
 * @param pipeline
 * @param buffer
 * @return SpiritResult
 */
SpiritResult spPipelineBindCommandBuffer(
    SpiritPipeline pipeline, SpiritCommandBuffer buffer);

/**
 * @brief Destroy a pipeline
 *
 * @param device
 * @param pipeline
 * @return SpiritResult
 */
SpiritResult spDestroyPipeline(SpiritDevice device, SpiritPipeline pipeline);
