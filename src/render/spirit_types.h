#pragma once

#include <cglm/cglm.h>
#include <stdint.h>

// Variables for the spirit render.
//
//
// Kael Johnston Feb 26 2022

//
// Definitions
//

#undef NULL
#define NULL ((void *)0)

// types
typedef unsigned char b8;
typedef int8_t i8;
typedef uint8_t u8;
typedef int16_t i16;
typedef uint16_t u16;
typedef int32_t i32;
typedef uint32_t u32;
typedef int64_t i64;
typedef uint64_t u64;
typedef float f32;
typedef double f64;

//
// Enumerations
//

typedef enum e_SpiritResult
{
    SPIRIT_SUCCESS   = 0, // total success
    SPIRIT_FAILURE   = 1, // certain failure
    SPIRIT_UNDEFINED = 2  // did not succeed, may not have failed
} SpiritResult;

//
// Structures
//

typedef struct t_SpiritResolution
{
    u64 w, h;
} SpiritResolution;

typedef char SpiritName[8];

/**
 * @brief Test of two names are equal.
 *
 * @param n1
 * @param n2
 * @return true - the names are the same
 * @return false - the names are different
 */
static __always_inline bool
SPIRIT_NAME_EQ(const SpiritName *n1, const SpiritName *n2)
{
    return *(u64*)n1 == *(u64*)n2;
}

//
// Component Types
//

typedef struct t_SpiritDevice *SpiritDevice;
typedef struct t_SpiritWindow *SpiritWindow;
typedef struct t_SpiritRenderPass *SpiritRenderPass;
typedef struct t_SpiritSwapchain *SpiritSwapchain;
typedef struct t_SpiritFence *SpiritFence;
typedef struct t_SpiritCommandBuffer *SpiritCommandBuffer;
typedef struct t_SpiritPipeline *SpiritPipeline;
typedef struct t_SpiritMaterial *SpiritMaterial;
typedef struct t_SpiritContext *SpiritContext;

typedef struct t_SpiritMesh *SpiritMesh;
typedef struct t_SpiritMeshManager *SpiritMeshManager;
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
typedef enum t_SpiritShaderType
{
    SPIRIT_SHADER_TYPE_AUTO_DETECT, // not recommended
    SPIRIT_SHADER_TYPE_VERTEX,
    SPIRIT_SHADER_TYPE_FRAGMENT,
    SPIRIT_SHADER_TYPE_COMPUTE,

    SPIRIT_SHADER_TYPE_MAX
} SpiritShaderType;

typedef u32 *SpiritShaderCode;

// store a vulkan (.spv) shader
typedef struct t_SpiritShader
{
    SpiritShaderType type;
    const char *path;
    SpiritShaderCode shader;
    u64 shaderSize;
} SpiritShader;

typedef struct t_SpiritPushConstant
{
    mat4 transform;
    CGLM_ALIGN(16) vec3 color;
} SpiritPushConstant;

// math presets
/* clang-format off */

#define SPIRIT_AXIS_UP       ((vec3) {0.f,  1.f,  0.f })
#define SPIRIT_AXIS_DOWN     ((vec3) {0.f,  -1.f, 0.f })
#define SPIRIT_AXIS_RIGHT    ((vec3) {1.f,  0.f,  0.f })
#define SPIRIT_AXIS_LEFT     ((vec3) {-1.f, 0.f,  0.f })
#define SPIRIT_AXIS_FORWARD  ((vec3) {0.f,  0.f,  1.f })
#define SPIRIT_AXIS_BACKWARD ((vec3) {0.f,  0.f,  -1.f})

/* clang-format on */