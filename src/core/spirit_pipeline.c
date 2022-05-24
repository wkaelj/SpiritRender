#include "spirit_pipeline.h"

// Implementation of spirit_pipeline.h
// 
// 
// Kael Johnston

// 
// Structures
//


typedef struct t_FixedFuncInfo
{
    VkViewport viewport;
    VkRect2D scissor;
    VkPipelineInputAssemblyStateCreateInfo inputAssemblyInfo;
    VkPipelineRasterizationStateCreateInfo rasterizationInfo;
    VkPipelineMultisampleStateCreateInfo multisampleInfo;
    VkPipelineColorBlendAttachmentState colorBlendAttachment;
    VkPipelineColorBlendStateCreateInfo colorBlendInfo;
    VkPipelineDepthStencilStateCreateInfo depthStencilInfo;
    VkPipelineLayout pipelineLayout;
    VkRenderPass renderPass;
    uint32_t subpass;
} FixedFuncInfo;

// 
// Helper functions
// 

// create a graphics pipeline
static VkPipeline createPipeline (
    SpiritPipelineCreateInfo *createInfo);

SpiritResult defaultPipelineConfig(FixedFuncInfo *pConfigInfo);

//
// Public Functions
//

SpiritPipeline spCreatePipeline(
    SpiritPipelineCreateInfo *createInfo,
    SpiritDevice device,
    SpiritSwapchain swapchain,
    // renderpasses?
    SpiritPipeline optionalPipeline)
{

}

// destroy a pipeline and free associated memory
SpiritResult spDestroyPipeline(
    SpiritPipeline pipeline,
    SpiritDevice device)
{

}

//
// Helper Function Implementation 
//

static VkPipeline createPipeline(
    SpiritPipelineCreateInfo *createInfo)
{

    VkPipeline pipeline = VK_NULL_HANDLE;

    FixedFuncInfo fixedInfo;
    defaultPipelineConfig(&fixedInfo);

    VkGraphicsPipelineCreateInfo pipelineCreateInfo = (VkGraphicsPipelineCreateInfo) {};


}

// the default piplinse configuration, should work for almost everything
SpiritResult defaultPipelineConfig(FixedFuncInfo *pConfigInfo)
{

    FixedFuncInfo configInfo = {};

    int32_t width, height;
    sveGetWindowSize(&width, &height);

    configInfo.inputAssemblyInfo = (VkPipelineInputAssemblyStateCreateInfo) {};
    configInfo.inputAssemblyInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
    configInfo.inputAssemblyInfo.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
    configInfo.inputAssemblyInfo.primitiveRestartEnable = VK_FALSE;

    configInfo.viewport = (VkViewport) {};
    configInfo.viewport.x = 0.0f;
    configInfo.viewport.y = 0.0f;
    configInfo.viewport.width = width;
    configInfo.viewport.height = height;
    configInfo.viewport.minDepth = 0.0f;
    configInfo.viewport.maxDepth = 1.0f;

    configInfo.scissor.offset.x = 0;
    configInfo.scissor.offset.y = 0;
    configInfo.scissor.extent.width = width;
    configInfo.scissor.extent.height = height;

    configInfo.rasterizationInfo = (VkPipelineRasterizationStateCreateInfo) {};
    configInfo.rasterizationInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
    configInfo.rasterizationInfo.depthClampEnable = VK_FALSE;
    configInfo.rasterizationInfo.rasterizerDiscardEnable = VK_FALSE;
    configInfo.rasterizationInfo.polygonMode = VK_POLYGON_MODE_FILL;
    configInfo.rasterizationInfo.lineWidth = 1.0f;
    configInfo.rasterizationInfo.cullMode = VK_CULL_MODE_NONE;
    configInfo.rasterizationInfo.frontFace = VK_FRONT_FACE_CLOCKWISE;
    configInfo.rasterizationInfo.depthBiasEnable = VK_FALSE;
    configInfo.rasterizationInfo.depthBiasConstantFactor = 0.0f; // Optional
    configInfo.rasterizationInfo.depthBiasClamp = 0.0f;          // Optional
    configInfo.rasterizationInfo.depthBiasSlopeFactor = 0.0f;    // Optional

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