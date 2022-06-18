#pragma once
#include <spirit_header.h>

// Variables for the spirit render.
// 
// 
// Kael Johnston Feb 26 2022

//
// Definitions
//

#undef NULL
#define NULL ((void*)0)

// types
#define i8  int8_t
#define u8  uint8_t
#define i16 int16_t
#define u16 uint16_t
#define i32 int32_t
#define u32 uint32_t
#define i64 int64_t
#define u64 uint64_t
#define f32 float
#define f64 double

//
// Enumerations
//

typedef enum e_SpiritResult {
    SPIRIT_SUCCESS = 0,  // total success
    SPIRIT_FAILURE = 1,  // certain failure
    SPIRIT_UNDEFINED = 2 // did not succeed, may not have failed
} SpiritResult;

//
// Structures
//

typedef struct t_SpiritResolution
{
    u64 w;
    u64 h;
} SpiritResolution;

typedef unsigned char byte;

// device information used by the swapchain code
// it is declared here so that neither the device or the swapchain need to include
// the other
typedef struct t_SpiritSwapchainSupportInfo {
    VkSurfaceCapabilitiesKHR capabilties;
    VkSurfaceFormatKHR      *formats; // maybe need to fix pointers later
    u32                      formatCount;
    VkPresentModeKHR        *presentModes;
    u32                      presentModeCount;
} SpiritSwapchainSupportInfo;

//
// Component Types
//

typedef struct t_SpiritDevice     *SpiritDevice;
typedef struct t_SpiritWindow     *SpiritWindow;
typedef struct t_SpiritRenderPass *SpiritRenderPass;
typedef struct t_SpiritSwapchain  *SpiritSwapchain;
typedef struct t_SpiritPipeline   *SpiritPipeline;
typedef struct t_SpiritMaterial   *SpiritMaterial;
typedef struct t_SpiritContext    *SpiritContext;

// shaders
// store wether a shader is frag or vert shader
typedef enum t_SpiritShaderType {
    SPIRIT_SHADER_TYPE_AUTO_DETECT, // not recommended
    SPIRIT_SHADER_TYPE_VERTEX,
    SPIRIT_SHADER_TYPE_FRAGMENT,
    SPIRIT_SHADER_TYPE_COMPUTE,

    SPIRIT_SHADER_TYPE_MAX
} SpiritShaderType;

typedef u32 *SpiritShaderCode;
// store a vulkan (.spv) shader
typedef struct t_SpiritShader {
    SpiritShaderType type;
    const char      *path;
    SpiritShaderCode shader;
    u64              shaderSize;
} SpiritShader;
