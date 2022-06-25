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
    VkPipelineInputAssemblyStateCreateInfo inputAssemblyInfo;
    VkPipelineRasterizationStateCreateInfo rasterizationInfo;
    VkPipelineMultisampleStateCreateInfo   multisampleInfo;
    VkPipelineColorBlendAttachmentState    colorBlendAttachment;
    VkPipelineColorBlendStateCreateInfo    colorBlendInfo;
    VkPipelineDepthStencilStateCreateInfo  depthStencilInfo;
    VkRenderPass                           renderPass;
    uint32_t                               subpass;
} FixedFuncInfo;

// 
// Helper functions
// 

// create a graphics pipeline
static VkPipeline createPipeline(
    SpiritDevice            device,
    SpiritRenderPass        renderPass,
    VkPipelineLayout        layout,
    const FixedFuncInfo    *fixedInfo,
    const u32               shaderCount,
    const VkShaderModule   *shaders,
    const SpiritShaderType *shaderTypes);

// creates a graphics pipeline layout
static VkPipelineLayout createLayout(SpiritDevice device);

// load array of shaders
// uses path and type from shaderInfo array
// outputs array of shader modules into dest
// dest must always be at least the side of moduleCount, otherwise it will fail
// - shaderInfo.path = "path/to/shader.vert" (no spv)
// - shaderInfo.type = SPIRIT_SHADER_TYPE_SHADER_TYPE
// - size of dest is size of module count, at least -> dest[moduleCount]
static SpiritResult loadShaderCode (
    SpiritDevice        device,
    VkShaderModule     *dest,
    const u32           moduleCount,
    const SpiritShader *shaderInfo);

// automaticaly handles options for pipeline creation
// using one big spagetti monster
static SpiritResult defaultPipelineConfig(
    SpiritPipelineCreateInfo *createInfo, 
    FixedFuncInfo *pConfigInfo);

//
// Public Functions
//

SpiritPipeline spCreatePipeline (
    const SpiritDevice        device,
    SpiritPipelineCreateInfo *createInfo,
    const SpiritSwapchain     swapchain,
    const SpiritRenderPass    renderPass,
    SpiritPipeline            optionalPipeline)
{

    SpiritPipeline pipeline = new_var (struct t_SpiritPipeline);

    // load shader modules
    u32 shaderCount = createInfo->shaderFilePathCount;
    VkShaderModule shaderCode[shaderCount];
    loadShaderCode (device, shaderCode, shaderCount, createInfo->shaderFilePaths);

    // update shader types
    SpiritShaderType shaderTypes[shaderCount];
    for (u32 i = 0; i < shaderCount; i++)
    {
        shaderTypes[i] = createInfo->shaderFilePaths[i].type;
    }

    pipeline->shaderCount = shaderCount;
    pipeline->shaders =     new_array (VkShaderModule, shaderCount);

    for (u32 i = 0; i < shaderCount; i++)
    {
        pipeline->shaders[i] = shaderCode[i];
    }

    // get config info
    FixedFuncInfo fixedInfo;
    defaultPipelineConfig (createInfo, &fixedInfo);

    pipeline->layout = createLayout(device);

    pipeline->pipeline = createPipeline (
        device,
        renderPass,
        pipeline->layout,
        &fixedInfo,
        pipeline->shaderCount,
        pipeline->shaders,
        shaderTypes);
    if (pipeline->pipeline == NULL)
    {
        free(pipeline->shaders);
        free(pipeline);
        return NULL;
    }

    return pipeline;
}

SpiritResult spPipelineBindCommandBuffer(
    SpiritPipeline pipeline,
    VkCommandBuffer buffer)
{
    if(pipeline == NULL || buffer == NULL)
    {
        log_warning("Cannot bind pipeline or buffers when one does not exist!");
        return SPIRIT_FAILURE;
    }
    
    vkCmdBindPipeline(buffer, VK_PIPELINE_BIND_POINT_GRAPHICS, pipeline->pipeline);

    return SPIRIT_SUCCESS;
}

// destroy a pipeline and free associated memory
SpiritResult spDestroyPipeline (
    const SpiritDevice device,
    SpiritPipeline     pipeline)
{

    if (!pipeline) return SPIRIT_FAILURE;

    vkDestroyPipelineLayout(device->device, pipeline->layout, NULL);

    vkDestroyPipeline (device->device, pipeline->pipeline, NULL);
    for (u32 i = 0; i < pipeline->shaderCount; i++)
    {
        vkDestroyShaderModule (device->device, pipeline->shaders[i], NULL);
    }
    dalloc (pipeline->shaders);
    dalloc (pipeline);

    return SPIRIT_SUCCESS;
}

VkPipelineLayout createLayout(SpiritDevice device)
{
    VkPipelineLayoutCreateInfo layoutInfo = {};
    layoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
    layoutInfo.setLayoutCount = 0;
    layoutInfo.pSetLayouts = NULL;
    layoutInfo.pushConstantRangeCount = 0;
    layoutInfo.pPushConstantRanges = NULL;

    VkPipelineLayout layout = VK_NULL_HANDLE;
    if (vkCreatePipelineLayout(device->device, &layoutInfo, NULL, &layout))
    {
        log_error("Failed to create pipeline layout");
        return NULL;
    }

    return layout;
}

//
// Helper Function Implementation 
//

static VkPipeline createPipeline(
    SpiritDevice            device,
    SpiritRenderPass        renderPass,
    VkPipelineLayout        layout,
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
            log_fatal ("Unable to recognize shader type %u", shaderTypes[i]);
            break;
        }

        SHADER.module =              shaders[i];
        SHADER.pName =              "main"; // entry function in glsl code
        SHADER.flags =               0; // no flags
        SHADER.pSpecializationInfo = NULL;
        SHADER.pNext =               NULL;

        #undef SHADER
    }

    VkVertexInputBindingDescription bindingDescription = spMeshGetBindingDescription();
    VkVertexInputAttributeDescription attributeDescription = spMeshGetAttributeDescription();

    // vertex input
    VkPipelineVertexInputStateCreateInfo vertInfo = {};
    vertInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
    vertInfo.vertexAttributeDescriptionCount = 1;
    vertInfo.pVertexAttributeDescriptions =    &attributeDescription;
    vertInfo.vertexBindingDescriptionCount =   1;
    vertInfo.pVertexBindingDescriptions =      &bindingDescription;


    VkPipelineViewportStateCreateInfo viewportInfo = {};
    viewportInfo.sType = 
        VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
    viewportInfo.viewportCount = 1;
    viewportInfo.pViewports =   &fixedInfo->viewport;
    viewportInfo.scissorCount =  1;
    viewportInfo.pScissors =    &fixedInfo->scissor;

    VkGraphicsPipelineCreateInfo pipelineInfo = (VkGraphicsPipelineCreateInfo) {};
    pipelineInfo.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
    pipelineInfo.stageCount =           shaderCount;
    pipelineInfo.pStages =              shaderCreateInfo;
    pipelineInfo.pVertexInputState =   &vertInfo;
    pipelineInfo.pInputAssemblyState = &fixedInfo->inputAssemblyInfo;
    pipelineInfo.pViewportState =      &viewportInfo;
    pipelineInfo.pMultisampleState =   &fixedInfo->multisampleInfo;
    pipelineInfo.pRasterizationState = &fixedInfo->rasterizationInfo;
    pipelineInfo.pColorBlendState =    &fixedInfo->colorBlendInfo;
    db_assert(
        pipelineInfo.pColorBlendState->pAttachments[0].blendEnable ==
        fixedInfo->colorBlendAttachment.blendEnable, "");

    pipelineInfo.pDepthStencilState =  &fixedInfo->depthStencilInfo;
    pipelineInfo.pDynamicState =        NULL;
    
    pipelineInfo.layout =     layout;
    pipelineInfo.renderPass = renderPass->renderPass;
    pipelineInfo.subpass =    0;

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

    log_verbose("Created pipeline");

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

    pConfigInfo->inputAssemblyInfo = (VkPipelineInputAssemblyStateCreateInfo) {};
    pConfigInfo->inputAssemblyInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
    pConfigInfo->inputAssemblyInfo.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
    pConfigInfo->inputAssemblyInfo.primitiveRestartEnable = VK_FALSE;

    pConfigInfo->viewport = (VkViewport) {};
    pConfigInfo->viewport.x = 0.0f;
    pConfigInfo->viewport.y = 0.0f;
    pConfigInfo->viewport.width = createInfo->resolution.w;
    pConfigInfo->viewport.height = createInfo->resolution.h;
    pConfigInfo->viewport.minDepth = 0.0f;
    pConfigInfo->viewport.maxDepth = 1.0f;

    pConfigInfo->scissor.offset.x = 0;
    pConfigInfo->scissor.offset.y = 0;
    pConfigInfo->scissor.extent.width = createInfo->resolution.w;
    pConfigInfo->scissor.extent.height = createInfo->resolution.h;


    pConfigInfo->rasterizationInfo = (VkPipelineRasterizationStateCreateInfo) {};
    pConfigInfo->rasterizationInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
    pConfigInfo->rasterizationInfo.depthClampEnable =        VK_FALSE;
    pConfigInfo->rasterizationInfo.rasterizerDiscardEnable = VK_FALSE;
    pConfigInfo->rasterizationInfo.polygonMode =             VK_POLYGON_MODE_FILL;
    pConfigInfo->rasterizationInfo.lineWidth =               1.0f;
    // TODO backface culling
    pConfigInfo->rasterizationInfo.cullMode =                VK_CULL_MODE_NONE;
    pConfigInfo->rasterizationInfo.frontFace =               VK_FRONT_FACE_CLOCKWISE;
    pConfigInfo->rasterizationInfo.depthBiasEnable =         VK_FALSE;
    pConfigInfo->rasterizationInfo.depthBiasConstantFactor = 0.0f; // Optional
    pConfigInfo->rasterizationInfo.depthBiasClamp =          0.0f; // Optional
    pConfigInfo->rasterizationInfo.depthBiasSlopeFactor =    0.0f; // Optional

    pConfigInfo->multisampleInfo = (VkPipelineMultisampleStateCreateInfo) {};
    pConfigInfo->multisampleInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
    pConfigInfo->multisampleInfo.sampleShadingEnable = VK_FALSE;
    pConfigInfo->multisampleInfo.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;
    pConfigInfo->multisampleInfo.minSampleShading = 1.0f;          // Optional
    pConfigInfo->multisampleInfo.pSampleMask = NULL;               // Optional
    pConfigInfo->multisampleInfo.alphaToCoverageEnable = VK_FALSE; // Optional
    pConfigInfo->multisampleInfo.alphaToOneEnable = VK_FALSE;      // Optional

    pConfigInfo->colorBlendAttachment = (VkPipelineColorBlendAttachmentState) {};
    pConfigInfo->colorBlendAttachment.blendEnable = VK_FALSE;
    pConfigInfo->colorBlendAttachment.colorWriteMask =
        VK_COLOR_COMPONENT_R_BIT |
        VK_COLOR_COMPONENT_G_BIT |
        VK_COLOR_COMPONENT_B_BIT |
        VK_COLOR_COMPONENT_A_BIT;
    pConfigInfo->colorBlendAttachment.blendEnable = VK_FALSE;
    pConfigInfo->colorBlendAttachment.srcColorBlendFactor = VK_BLEND_FACTOR_ONE; // Optional
    pConfigInfo->colorBlendAttachment.dstColorBlendFactor = VK_BLEND_FACTOR_ZERO;// Optional
    pConfigInfo->colorBlendAttachment.colorBlendOp =        VK_BLEND_OP_ADD; // Optional
    pConfigInfo->colorBlendAttachment.srcAlphaBlendFactor = VK_BLEND_FACTOR_ONE; // Optional
    pConfigInfo->colorBlendAttachment.dstAlphaBlendFactor = VK_BLEND_FACTOR_ZERO;// Optional
    pConfigInfo->colorBlendAttachment.alphaBlendOp =        VK_BLEND_OP_ADD; // Optional

    pConfigInfo->colorBlendInfo = (VkPipelineColorBlendStateCreateInfo) {};
    pConfigInfo->colorBlendInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
    pConfigInfo->colorBlendInfo.logicOpEnable = VK_FALSE;
    pConfigInfo->colorBlendInfo.logicOp = VK_LOGIC_OP_COPY; // Optional
    pConfigInfo->colorBlendInfo.attachmentCount = 1;
    pConfigInfo->colorBlendInfo.pAttachments = &pConfigInfo->colorBlendAttachment;
    pConfigInfo->colorBlendInfo.blendConstants[0] = 0.0f; // Optional
    pConfigInfo->colorBlendInfo.blendConstants[1] = 0.0f; // Optional
    pConfigInfo->colorBlendInfo.blendConstants[2] = 0.0f; // Optional
    pConfigInfo->colorBlendInfo.blendConstants[3] = 0.0f; // Optional

    pConfigInfo->depthStencilInfo = (VkPipelineDepthStencilStateCreateInfo) {};
    pConfigInfo->depthStencilInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO;
    pConfigInfo->depthStencilInfo.depthTestEnable = VK_TRUE;
    pConfigInfo->depthStencilInfo.depthWriteEnable = VK_TRUE;
    pConfigInfo->depthStencilInfo.depthCompareOp = VK_COMPARE_OP_LESS;
    pConfigInfo->depthStencilInfo.depthBoundsTestEnable = VK_FALSE;
    pConfigInfo->depthStencilInfo.minDepthBounds = 0.0f; // Optional
    pConfigInfo->depthStencilInfo.maxDepthBounds = 1.0f; // Optional
    pConfigInfo->depthStencilInfo.stencilTestEnable = VK_FALSE;
    // pConfigInfo->depthStencilInfo.front = {};  // Optional
    // pConfigInfo->depthStencilInfo.back = {};   // Optional

    return SPIRIT_SUCCESS;
}
