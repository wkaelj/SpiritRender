#pragma once
#include <spirit_header.h>
#include "spirit_renderpass.h"
#include "spirit_pipeline.h"
#include "spirit_mesh.h"
#include "spirit_context.h"

// materials have their own render pass, pipeline and shaders
// which allow them to be completely customizable
// shaders work by adding a meshrender component to a material
// and then that mesh will be rendered by the shader that frame

typedef struct t_SpiritMaterialCreateInfo
{
    const char *name;
    const char *vertexShader;
    const char *fragmentShader;

} SpiritMaterialCreateInfo;

// used internally, do not reference
typedef struct t_SpiritMaterialMeshList *t_SpiritMaterialMeshList;

struct t_SpiritMaterial
{
    const char *name;
    const char *vertexShader;
    const char *fragmentShader;

    SpiritRenderPass renderPass;
    SpiritPipeline pipeline;

    struct t_SpiritMaterialMeshList *list;
};

SpiritMaterial spCreateMaterial(
    const SpiritContext context,
    const SpiritMaterialCreateInfo *createInfo);

SpiritResult spMaterialAddMesh(
    const SpiritMaterial material,
    const SpiritMeshReference meshRef);

// will submit all commands to draw associated meshes, and will
// release references to all meshes
// returns the number of vertices submit
size_t spMaterialRecordCommands(
    const SpiritContext context,
    SpiritMaterial material);

SpiritResult spDestroyMaterial(
    const SpiritContext context,
    SpiritMaterial material);
