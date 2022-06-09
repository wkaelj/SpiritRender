#include "spirit_pipeline.h"
#include <glsl-loader/glsl_loader.h>

// Implementation of spirit_pipeline.h
// 
// 
// Kael Johnston

// 
// Structures
//


typedef struct t_FixedFuncInfo
{
    VkViewport                             viewport;
    VkRect2D                               scissor;
    VkPipelineViewportStateCreateInfo      viewportInfo;
    VkPipelineInputAssemblyStateCreateInfo inputAssemblyInfo;
    VkPipelineRasterizationStateCreateInfo rasterizationInfo;
    VkPipelineMultisampleStateCreateInfo   multisampleInfo;
    VkPipelineColorBlendAttachmentState    colorBlendAttachment;
    VkPipelineColorBlendStateCreateInfo    colorBlendInfo;
    VkPipelineDepthStencilStateCreateInfo  depthStencilInfo;
    VkPipelineLayout                       pipelineLayout;
    VkRenderPass                           renderPass;
    uint32_t                               subpass;
} FixedFuncInfo;

// 
// Helper functions
// 

// create a graphics pipeline
static VkPipeline createPipeline(
    SpiritDevice            device,
    const FixedFuncInfo    *fixedInfo,
    const u32               shaderCount,
    const VkShaderModule   *shaders,
    const SpiritShaderType *shaderTypes);

// load array of shaders
// uses path and type from shaderInfo array
// outputs array of shader modules into dest
// dest must always be at least the side of moduleCount, otherwise it will fail
// - shaderInfo.path = "path/to/shader.vert" (no spv)
// - shaderInfo.type = SPIRIT_SHADER_TYPE_SHADER_TYPE
// - size of dest is size of module count, at least -> dest[moduleCount]
SpiritResult loadShaderCode (
    SpiritDevice        device,
    VkShaderModule     *dest,
    const u32           moduleCount,
    const SpiritShader *shaderInfo);

SpiritResult defaultPipelineConfig(
    SpiritPipelineCreateInfo *createInfo, 
    FixedFuncInfo *pConfigInfo);

//
// Public Functions
//

SpiritPipeline spCreatePipeline (
    const SpiritDevice        device,
    SpiritPipelineCreateInfo *createInfo,
    const SpiritSwapchain     swapchain,
    // renderpasses?
    SpiritPipeline            optionalPipeline)
{

    SpiritPipeline pipeline = new_var (struct t_SpiritPipeline);

    // load shader modules
    u32 shaderCount = createInfo->shaderFilePathCount;
    VkShaderModule shaderCode[shaderCount];
    loadShaderCode (device, shaderCode, shaderCount, createInfo->shaderFilePaths);
    SpiritShaderType shaderTypes[shaderCount];

    pipeline->shaderCount = shaderCount;
    pipeline->shaders =     new_array (VkShaderModule, shaderCount);

    for (u32 i = 0; i < shaderCount; i++)
    {
        pipeline->shaders[i] = shaderCode[i];
    }

    // get config info
    FixedFuncInfo fixedInfo;
    defaultPipelineConfig (createInfo, &fixedInfo);

    pipeline->pipeline = createPipeline (
        device,
        &fixedInfo,
        pipeline->shaderCount,
        pipeline->shaders,
        shaderTypes);

    return pipeline;
}

// destroy a pipeline and free associated memory
SpiritResult spDestroyPipeline (
    const SpiritDevice device,
    SpiritPipeline     pipeline)
{

    if (!pipeline) return SPIRIT_FAILURE;

    vkDestroyPipeline (device->device, pipeline->pipeline, NULL);
    for (u32 i = 0; i < pipeline->shaderCount; i++)
    {
        vkDestroyShaderModule (device->device, pipeline->shaders[i], NULL);
    }
    dalloc (pipeline->shaders);
    dalloc (pipeline);

    return SPIRIT_SUCCESS;
}

//
// Helper Function Implementation 
//

static VkPipeline createPipeline(
    SpiritDevice            device,
    const FixedFuncInfo    *fixedInfo,
    const u32               shaderCount,
    const VkShaderModule   *shaders,
    const SpiritShaderType *shaderTypes)
{


    // configure shader info for each shader
    VkPipelineShaderStageCreateInfo shaderCreateInfo[shaderCount];
    for (u32 i = 0; i < shaderCount; i++)
    {
        #define SHADER shaderCreateInfo[i]

        SHADER = (VkPipelineShaderStageCreateInfo) {};
        SHADER.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;

        switch (shaderTypes[i])
        {
        case SPIRIT_SHADER_TYPE_VERTEX:
            SHADER.stage = VK_SHADER_STAGE_VERTEX_BIT;
            break;
        case SPIRIT_SHADER_TYPE_FRAGMENT:
            SHADER.stage = VK_SHADER_STAGE_FRAGMENT_BIT;
            break;
        // TODO compute shader
        case SPIRIT_SHADER_TYPE_COMPUTE:
            log_fatal ("Compute shaders are not supported! yet...");
            abort();
            break;
        default:
            log_fatal ("Unable to recognize shader type");
            break;
        }

        SHADER.module =              shaders[i];
        SHADER.pName =              "main"; // entry function in glsl code
        SHADER.flags =               0; // no flags
        SHADER.pSpecializationInfo = NULL;
        SHADER.pNext =               NULL;

        #undef SHADER
    }

    // vertex input
    VkPipelineVertexInputStateCreateInfo vertInfo = {};
    vertInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
    vertInfo.vertexAttributeDescriptionCount = 0;
    vertInfo.pVertexAttributeDescriptions =    NULL;
    vertInfo.vertexBindingDescriptionCount =   0;
    vertInfo.pVertexBindingDescriptions =      NULL;

    VkGraphicsPipelineCreateInfo pipelineInfo = (VkGraphicsPipelineCreateInfo) {};
    pipelineInfo.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
    pipelineInfo.stageCount =           shaderCount;
    pipelineInfo.pStages =              shaderCreateInfo;
    pipelineInfo.pVertexInputState =   &vertInfo;
    pipelineInfo.pInputAssemblyState = &fixedInfo->inputAssemblyInfo;
    pipelineInfo.pViewportState =      &fixedInfo->viewportInfo;
    pipelineInfo.pRasterizationState = &fixedInfo->rasterizationInfo;
    pipelineInfo.pColorBlendState =    &fixedInfo->colorBlendInfo;
    pipelineInfo.pDepthStencilState =  &fixedInfo->depthStencilInfo;
    pipelineInfo.pDynamicState =        NULL;
    
    pipelineInfo.layout =     fixedInfo->pipelineLayout;
    pipelineInfo.renderPass = fixedInfo->renderPass;
    pipelineInfo.subpass =    fixedInfo->subpass;

    pipelineInfo.basePipelineIndex = -1;
    pipelineInfo.basePipelineHandle = NULL;

    VkPipeline pipeline = NULL;

    if (vkCreateGraphicsPipelines (
        device->device, 
        NULL, 
        1, 
        &pipelineInfo,
        NULL,
        &pipeline) != VK_SUCCESS)
    {
        log_error("Failed to create graphics pipeline");
        return NULL;
    }

    return pipeline;
}

// load array of shaders
// uses path and type from shaderInfo array
// outputs array of shader modules into dest
// dest must always be at least the side of moduleCount, otherwise it will fail
// - shaderInfo.path = "path/to/shader.vert" (no spv)
// - shaderInfo.type = SPIRIT_SHADER_TYPE_SHADER_TYPE
// - size of dest is size of module count, at least -> dest[moduleCount]
SpiritResult loadShaderCode (
    SpiritDevice        device,
    VkShaderModule     *dest,
    const u32           moduleCount,
    const SpiritShader *shaderInfo)
{

    // store the error value in this and continue attemting to compile shaders
    // so at least most of the shaders get compiled in the event of a failure
    SpiritResult error = SPIRIT_SUCCESS;

    for (u32 i = 0; i < moduleCount; i++)
    {
        // load shader into s, to be error checked
        SpiritShader s = (SpiritShader) {};
        s = spLoadSourceShader (shaderInfo[i].path, shaderInfo[i].type);

        // error
        if (s.shaderSize == 0)
        {
            log_error ("Failed to load shader '%s', index %u",
                shaderInfo[i].path, i);
            return SPIRIT_FAILURE;
        }

        // create shader module
        if ((dest[i] = spConvertShaderToModule(device, &s)) == NULL) error = SPIRIT_FAILURE;
        spDestroyShader(s); // clean up loaded shader code
    }

    return error;
} // loadShaderCode

// the default piplinse configuration, should work for almost everything
SpiritResult defaultPipelineConfig(
    SpiritPipelineCreateInfo *createInfo, 
    FixedFuncInfo *pConfigInfo)
{

    FixedFuncInfo configInfo = {};

    configInfo.inputAssemblyInfo = (VkPipelineInputAssemblyStateCreateInfo) {};
    configInfo.inputAssemblyInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
    configInfo.inputAssemblyInfo.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
    configInfo.inputAssemblyInfo.primitiveRestartEnable = VK_FALSE;

    configInfo.viewport = (VkViewport) {};
    configInfo.viewport.x = 0.0f;
    configInfo.viewport.y = 0.0f;
    configInfo.viewport.width = createInfo->windowWidth;
    configInfo.viewport.height = createInfo->windowWidth;
    configInfo.viewport.minDepth = 0.0f;
    configInfo.viewport.maxDepth = 1.0f;

    configInfo.scissor.offset.x = 0;
    configInfo.scissor.offset.y = 0;
    configInfo.scissor.extent.width = createInfo->windowWidth;
    configInfo.scissor.extent.height = createInfo->windowHeight;

    configInfo.viewportInfo = (VkPipelineViewportStateCreateInfo) {};
    configInfo.viewportInfo.viewportCount = 1;
    configInfo.viewportInfo.pViewports =   &configInfo.viewport;
    configInfo.viewportInfo.scissorCount =  1;
    configInfo.viewportInfo.pScissors =    &configInfo.scissor;

    configInfo.rasterizationInfo = (VkPipelineRasterizationStateCreateInfo) {};
    configInfo.rasterizationInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
    configInfo.rasterizationInfo.depthClampEnable =        VK_FALSE;
    configInfo.rasterizationInfo.rasterizerDiscardEnable = VK_FALSE;
    configInfo.rasterizationInfo.polygonMode =             VK_POLYGON_MODE_FILL;
    configInfo.rasterizationInfo.lineWidth =               1.0f;
    // TODO backface culling
    configInfo.rasterizationInfo.cullMode =                VK_CULL_MODE_NONE;
    configInfo.rasterizationInfo.frontFace =               VK_FRONT_FACE_CLOCKWISE;
    configInfo.rasterizationInfo.depthBiasEnable =         VK_FALSE;
    configInfo.rasterizationInfo.depthBiasConstantFactor = 0.0f; // Optional
    configInfo.rasterizationInfo.depthBiasClamp =          0.0f; // Optional
    configInfo.rasterizationInfo.depthBiasSlopeFactor =    0.0f; // Optional

    configInfo.multisampleInfo = (VkPipelineMultisampleStateCreateInfo) {};
    configInfo.multisampleInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
    configInfo.multisampleInfo.sampleShadingEnable = VK_FALSE;
    configInfo.multisampleInfo.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;
    configInfo.multisampleInfo.minSampleShading = 1.0f;          // Optional
    configInfo.multisampleInfo.pSampleMask = NULL;               // Optional
    configInfo.multisampleInfo.alphaToCoverageEnable = VK_FALSE; // Optional
    configInfo.multisampleInfo.alphaToOneEnable = VK_FALSE;      // Optional

    configInfo.colorBlendAttachment = (VkPipelineColorBlendAttachmentState) {};
    configInfo.colorBlendAttachment.colorWriteMask =
        VK_COLOR_COMPONENT_R_BIT |
        VK_COLOR_COMPONENT_G_BIT |
        VK_COLOR_COMPONENT_B_BIT |
        VK_COLOR_COMPONENT_A_BIT;
    configInfo.colorBlendAttachment.blendEnable = VK_FALSE;
    configInfo.colorBlendAttachment.srcColorBlendFactor = 
        VK_BLEND_FACTOR_ONE;  // Optional
    configInfo.colorBlendAttachment.dstColorBlendFactor = 
        VK_BLEND_FACTOR_ZERO; // Optional
    configInfo.colorBlendAttachment.colorBlendOp = 
        VK_BLEND_OP_ADD;             // Optional
    configInfo.colorBlendAttachment.srcAlphaBlendFactor = 
        VK_BLEND_FACTOR_ONE;  // Optional
    configInfo.colorBlendAttachment.dstAlphaBlendFactor = 
        VK_BLEND_FACTOR_ZERO; // Optional
    configInfo.colorBlendAttachment.alphaBlendOp = 
        VK_BLEND_OP_ADD;             // Optional

    configInfo.colorBlendInfo = (VkPipelineColorBlendStateCreateInfo) {};
    configInfo.colorBlendInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
    configInfo.colorBlendInfo.logicOpEnable = VK_FALSE;
    configInfo.colorBlendInfo.logicOp = VK_LOGIC_OP_COPY; // Optional
    configInfo.colorBlendInfo.attachmentCount = 1;
    configInfo.colorBlendInfo.pAttachments = &configInfo.colorBlendAttachment;
    configInfo.colorBlendInfo.blendConstants[0] = 0.0f; // Optional
    configInfo.colorBlendInfo.blendConstants[1] = 0.0f; // Optional
    configInfo.colorBlendInfo.blendConstants[2] = 0.0f; // Optional
    configInfo.colorBlendInfo.blendConstants[3] = 0.0f; // Optional

    configInfo.depthStencilInfo = (VkPipelineDepthStencilStateCreateInfo) {};
    configInfo.depthStencilInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO;
    configInfo.depthStencilInfo.depthTestEnable = VK_TRUE;
    configInfo.depthStencilInfo.depthWriteEnable = VK_TRUE;
    configInfo.depthStencilInfo.depthCompareOp = VK_COMPARE_OP_LESS;
    configInfo.depthStencilInfo.depthBoundsTestEnable = VK_FALSE;
    configInfo.depthStencilInfo.minDepthBounds = 0.0f; // Optional
    configInfo.depthStencilInfo.maxDepthBounds = 1.0f; // Optional
    configInfo.depthStencilInfo.stencilTestEnable = VK_FALSE;
    // configInfo.depthStencilInfo.front = {};  // Optional
    // configInfo.depthStencilInfo.back = {};   // Optional

    *pConfigInfo = configInfo;

    return SPIRIT_SUCCESS;
}