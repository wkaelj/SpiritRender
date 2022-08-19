#pragma once

#include <stdint.h>

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
typedef int8_t   i8;
typedef uint8_t  u8;
typedef int16_t  i16;
typedef uint16_t u16;
typedef int32_t  i32;
typedef uint32_t u32;
typedef int64_t  i64;
typedef uint64_t u64;
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

//
// Component Types
//

typedef struct t_SpiritDevice     *SpiritDevice;
typedef struct t_SpiritWindow     *SpiritWindow;
typedef struct t_SpiritRenderPass *SpiritRenderPass;
typedef struct t_SpiritSwapchain  *SpiritSwapchain;
typedef struct t_SpiritFence      *SpiritFence;
typedef struct t_SpiritCommandBuffer *SpiritCommandBuffer;
typedef struct t_SpiritPipeline   *SpiritPipeline;
typedef struct t_SpiritMaterial   *SpiritMaterial;
typedef struct t_SpiritContext    *SpiritContext;

typedef struct t_SpiritMesh         *SpiritMesh;
typedef struct t_SpiritMeshManager  *SpiritMeshManager;
// a reference to a mesh stored in a mesh manager
// can be obtained and released from
typedef struct t_SpiritMeshReference
{
    struct t_MeshListNode *node;
    u32 vertCount;
    SpiritMeshManager meshManager;
} SpiritMeshReference;

typedef struct t_SpiritImage SpiritImage;

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
