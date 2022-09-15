#include "spirit_pipeline.h"
#include "core/spirit_material.h"
#include "core/spirit_mesh.h"
#include "spirit_command_buffer.h"
#include "spirit_device.h"
#include "spirit_renderpass.h"
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
    VkRect2D scissor;
    VkPipelineViewportStateCreateInfo viewportInfo;
    VkPipelineInputAssemblyStateCreateInfo inputAssemblyInfo;
    VkPipelineRasterizationStateCreateInfo rasterizationInfo;
    VkPipelineMultisampleStateCreateInfo multisampleInfo;
    VkPipelineColorBlendAttachmentState colorBlendAttachment;
    VkPipelineColorBlendStateCreateInfo colorBlendInfo;
    VkPipelineDepthStencilStateCreateInfo depthStencilInfo;
    VkDynamicState dynamicStateEnables[2];
    VkPipelineDynamicStateCreateInfo dynamicStateInfo;
    VkRenderPass renderPass;
    uint32_t subpass;
} FixedFuncInfo;

//
// Helper functions
//

// create a graphics pipeline
static VkPipeline createPipeline(
    SpiritDevice device,
    SpiritRenderPass renderPass,
    VkPipelineLayout layout,
    const FixedFuncInfo *fixedInfo,
    const VkShaderModule vertexShader,
    const VkShaderModule fragmentShader);

// creates a graphics pipeline layout
static VkPipelineLayout createLayout(SpiritDevice device);

// load array of shaders
// uses path and type from shaderInfo array
// outputs array of shader modules into dest
// dest must always be at least the side of moduleCount, otherwise it will fail
// - shaderInfo.path = "path/to/shader.vert" (no spv)
// - shaderInfo.type = SPIRIT_SHADER_TYPE_SHADER_TYPE
// - size of dest is size of module count, at least -> dest[moduleCount]
static SpiritResult loadShaderCode(
    SpiritDevice device,
    VkShaderModule *dest,
    const char *path,
    const SpiritShaderType shaderType);

// automaticaly handles options for pipeline creation
// using one big spagetti monster
static void defaultPipelineConfig(
    const SpiritPipelineCreateInfo *createInfo, FixedFuncInfo *pConfigInfo);

//
// Public Functions
//

SpiritPipeline spCreatePipeline(
    const SpiritDevice device,
    SpiritPipelineCreateInfo *createInfo,
    const SpiritRenderPass renderPass,
    SpiritPipeline optionalPipeline __attribute_maybe_unused__)
{

    SpiritPipeline pipeline = new_var(struct t_SpiritPipeline);

    // load shader modules
    VkShaderModule vertexShader, fragmentShader;
    if (loadShaderCode(
            device,
            &vertexShader,
            createInfo->vertexShader,
            SPIRIT_SHADER_TYPE_VERTEX))
    {
        log_error("Failed to load shader '%s'", createInfo->vertexShader);
        return NULL;
    }
    if (loadShaderCode(
            device,
            &fragmentShader,
            createInfo->fragmentShader,
            SPIRIT_SHADER_TYPE_FRAGMENT))
    {
        log_error("Failed to load shader '%s'", createInfo->fragmentShader);
        return NULL;
    }

    // get config info
    FixedFuncInfo fixedInfo;
    defaultPipelineConfig(createInfo, &fixedInfo);

    pipeline->layout = createLayout(device);
    if (pipeline->layout == NULL)
    {
        log_error("Failed to create pipeline layout");
        vkDestroyShaderModule(device->device, vertexShader, NULL);
        vkDestroyShaderModule(device->device, fragmentShader, NULL);
        free(pipeline);
        return NULL;
    }

    pipeline->pipeline = createPipeline(
        device,
        renderPass,
        pipeline->layout,
        &fixedInfo,
        vertexShader,
        fragmentShader);

    if (pipeline->pipeline == NULL)
    {
        log_error("Failed to create pipeline");
        vkDestroyPipelineLayout(device->device, pipeline->layout, NULL);

        vkDestroyShaderModule(device->device, vertexShader, NULL);
        vkDestroyShaderModule(device->device, fragmentShader, NULL);

        free(pipeline);
        return NULL;
    }

    vkDestroyShaderModule(device->device, vertexShader, NULL);
    vkDestroyShaderModule(device->device, fragmentShader, NULL);

    return pipeline;
}

SpiritResult
spPipelineBindCommandBuffer(SpiritPipeline pipeline, SpiritCommandBuffer buffer)
{
    if (pipeline == NULL || buffer == NULL)
    {
        log_warning("Cannot bind pipeline or buffers when one does not exist!");
        return SPIRIT_FAILURE;
    }

    vkCmdBindPipeline(
        buffer->handle, VK_PIPELINE_BIND_POINT_GRAPHICS, pipeline->pipeline);

    return SPIRIT_SUCCESS;
}

// destroy a pipeline and free associated memory
SpiritResult
spDestroyPipeline(const SpiritDevice device, SpiritPipeline pipeline)
{

    if (!pipeline)
        return SPIRIT_FAILURE;

    vkDestroyPipelineLayout(device->device, pipeline->layout, NULL);
    vkDestroyPipeline(device->device, pipeline->pipeline, NULL);
    free(pipeline);

    return SPIRIT_SUCCESS;
}


//
// Helper Function Implementation
//

VkPipelineLayout createLayout(SpiritDevice device)
{

    VkPushConstantRange pushRanges = {
        .offset = 0,
        .size = sizeof(SpiritPushConstant),
        .stageFlags = VK_SHADER_STAGE_VERTEX_BIT
    };

    VkPipelineLayoutCreateInfo layoutInfo = {};
    layoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
    layoutInfo.setLayoutCount = 0;
    layoutInfo.pSetLayouts = NULL;
    layoutInfo.pushConstantRangeCount = 0;
    layoutInfo.pPushConstantRanges = NULL;
    layoutInfo.sType          = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
    layoutInfo.setLayoutCount = 0;
    layoutInfo.pSetLayouts    = NULL;
    layoutInfo.pushConstantRangeCount = 1;
    layoutInfo.pPushConstantRanges    = &pushRanges;

    VkPipelineLayout layout = VK_NULL_HANDLE;
    if (vkCreatePipelineLayout(device->device, &layoutInfo, NULL, &layout))
    {
        return NULL;
    }

    return layout;
}

VkPipeline createPipeline(
    SpiritDevice device,
    SpiritRenderPass renderPass,
    VkPipelineLayout layout,
    const FixedFuncInfo *fixedInfo,
    const VkShaderModule vertexShader,
    const VkShaderModule fragmentShader)
{

    // configure shader info for each shader
    VkPipelineShaderStageCreateInfo shaderCreateInfo[2];

    shaderCreateInfo[0] = (VkPipelineShaderStageCreateInfo){};
    shaderCreateInfo[0].sType =
        VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
    shaderCreateInfo[0].module = vertexShader;
    shaderCreateInfo[0].pName  = "main"; // entry function in glsl code
    shaderCreateInfo[0].stage  = VK_SHADER_STAGE_VERTEX_BIT;
    shaderCreateInfo[1]        = (VkPipelineShaderStageCreateInfo){};
    shaderCreateInfo[1].sType =
        VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
    shaderCreateInfo[1].module = fragmentShader;
    shaderCreateInfo[1].pName  = "main";
    shaderCreateInfo[1].stage  = VK_SHADER_STAGE_FRAGMENT_BIT;

    VkVertexInputBindingDescription bindingDescription =
        spMeshGetBindingDescription();
    VkVertexInputAttributeDescription attributeDescription =
        spMeshGetAttributeDescription();

    // vertex input
    VkPipelineVertexInputStateCreateInfo vertInfo = {};
    vertInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
    vertInfo.vertexAttributeDescriptionCount = 1;
    vertInfo.pVertexAttributeDescriptions    = &attributeDescription;
    vertInfo.vertexBindingDescriptionCount   = 1;
    vertInfo.pVertexBindingDescriptions      = &bindingDescription;

    VkGraphicsPipelineCreateInfo pipelineInfo =
        (VkGraphicsPipelineCreateInfo){};
    pipelineInfo.sType      = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
    pipelineInfo.stageCount = 2;
    pipelineInfo.pStages    = shaderCreateInfo;
    pipelineInfo.pVertexInputState   = &vertInfo;
    pipelineInfo.pInputAssemblyState = &fixedInfo->inputAssemblyInfo;
    pipelineInfo.pViewportState      = &fixedInfo->viewportInfo;
    pipelineInfo.pMultisampleState   = &fixedInfo->multisampleInfo;
    pipelineInfo.pRasterizationState = &fixedInfo->rasterizationInfo;
    pipelineInfo.pColorBlendState    = &fixedInfo->colorBlendInfo;
    db_assert_msg(
        pipelineInfo.pColorBlendState->pAttachments[0].blendEnable ==
            fixedInfo->colorBlendAttachment.blendEnable,
        "");

    pipelineInfo.pDepthStencilState = &fixedInfo->depthStencilInfo;
    pipelineInfo.pDynamicState      = &fixedInfo->dynamicStateInfo;

    pipelineInfo.layout     = layout;
    pipelineInfo.renderPass = renderPass->renderPass;
    pipelineInfo.subpass    = 0;

    pipelineInfo.basePipelineIndex  = -1;
    pipelineInfo.basePipelineHandle = NULL;

    VkPipeline pipeline = NULL;

    if (vkCreateGraphicsPipelines(
            device->device, NULL, 1, &pipelineInfo, NULL, &pipeline) !=
        VK_SUCCESS)
    {
        return NULL;
    }

    return pipeline;
}

SpiritResult loadShaderCode(
    SpiritDevice device,
    VkShaderModule *dest,
    const char *path,
    const SpiritShaderType shaderType)
{

    // load shader into s, to be error checked
    SpiritShader s = (SpiritShader){};
    s              = spLoadSourceShader(path, shaderType);

    // error
    if (s.shaderSize == 0)
    {
        log_error("Failed to load shader '%s'", path);
        return SPIRIT_FAILURE;
    }

    // create shader module
    *dest = spConvertShaderToModule(device, &s);
    spDestroyShader(s);

    if (*dest == NULL)
    {
        return SPIRIT_FAILURE;
    }

    return SPIRIT_SUCCESS;
}

// the default piplinse configuration, should work for almost everything
void defaultPipelineConfig(
    const SpiritPipelineCreateInfo *createInfo, FixedFuncInfo *pConfigInfo)
{

    pConfigInfo->inputAssemblyInfo = (VkPipelineInputAssemblyStateCreateInfo){
        .sType    = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO,
        .topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST,
        .primitiveRestartEnable = VK_FALSE};

    pConfigInfo->rasterizationInfo = (VkPipelineRasterizationStateCreateInfo){
        .sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO,
        .depthClampEnable        = VK_FALSE,
        .rasterizerDiscardEnable = VK_FALSE,
        .polygonMode             = VK_POLYGON_MODE_FILL,
        .lineWidth               = 1.0f,
        // TODO backface culling
        .cullMode                = VK_CULL_MODE_NONE,
        .frontFace               = VK_FRONT_FACE_CLOCKWISE,
        .depthBiasEnable         = VK_FALSE,
        .depthBiasConstantFactor = 0.0f,
        .depthBiasClamp          = 0.0f,
        .depthBiasSlopeFactor    = 0.0f};

    pConfigInfo->viewportInfo = (VkPipelineViewportStateCreateInfo){
        .sType         = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO,
        .viewportCount = 1,
        .pViewports    = NULL,
        .scissorCount  = 1,
        .pScissors     = NULL,
    };

    pConfigInfo->multisampleInfo = (VkPipelineMultisampleStateCreateInfo){
        .sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO,
        .sampleShadingEnable   = VK_FALSE,
        .rasterizationSamples  = VK_SAMPLE_COUNT_1_BIT,
        .minSampleShading      = 1.0f,
        .pSampleMask           = NULL,
        .alphaToCoverageEnable = VK_FALSE,
        .alphaToOneEnable      = VK_FALSE};

    pConfigInfo->colorBlendAttachment = (VkPipelineColorBlendAttachmentState){
        .blendEnable    = VK_FALSE,
        .colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT |
                          VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT,
        .srcColorBlendFactor = VK_BLEND_FACTOR_ONE,
        .dstColorBlendFactor = VK_BLEND_FACTOR_ZERO,
        .colorBlendOp        = VK_BLEND_OP_ADD,
        .srcAlphaBlendFactor = VK_BLEND_FACTOR_ONE,
        .dstAlphaBlendFactor = VK_BLEND_FACTOR_ZERO,
        .alphaBlendOp        = VK_BLEND_OP_ADD,

    };

    pConfigInfo->colorBlendInfo = (VkPipelineColorBlendStateCreateInfo){
        .sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO,
        .logicOpEnable     = VK_FALSE,
        .logicOp           = VK_LOGIC_OP_COPY,
        .attachmentCount   = 1,
        .pAttachments      = &pConfigInfo->colorBlendAttachment,
        .blendConstants[0] = 0.0f,
        .blendConstants[1] = 0.0f,
        .blendConstants[2] = 0.0f,
        .blendConstants[3] = 0.0f};

    pConfigInfo->depthStencilInfo = (VkPipelineDepthStencilStateCreateInfo){
        .sType = VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO,
        .depthTestEnable       = VK_TRUE,
        .depthWriteEnable      = VK_TRUE,
        .depthCompareOp        = VK_COMPARE_OP_LESS,
        .depthBoundsTestEnable = VK_FALSE,
        .minDepthBounds        = 0.0f,
        .maxDepthBounds        = 1.0f,
        .stencilTestEnable     = VK_FALSE
        // .front = {};  // Optional
        // .back = {};   // Optional
    };

    pConfigInfo->dynamicStateEnables[0] = VK_DYNAMIC_STATE_VIEWPORT;
    pConfigInfo->dynamicStateEnables[1] = VK_DYNAMIC_STATE_SCISSOR;

    pConfigInfo->dynamicStateInfo = (VkPipelineDynamicStateCreateInfo){};
    pConfigInfo->dynamicStateInfo = (VkPipelineDynamicStateCreateInfo){
        .sType          = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO,
        .pDynamicStates = pConfigInfo->dynamicStateEnables,
        .dynamicStateCount = array_length(pConfigInfo->dynamicStateEnables),
        .flags             = 0};
}