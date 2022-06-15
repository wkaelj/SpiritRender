#pragma once
#include <spirit_header.h>
#include "spirit_renderpass.h"
#include "spirit_pipeline.h"

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

SpiritMaterial spCreateMaterial(
    const SpiritContext context,
    const SpiritMaterialCreateInfo *createInfo);

SpiritResult spMaterialAddMesh(
    const SpiritContext context);

SpiritResult spMaterialRecordCommands(
    const SpiritContext context);

SpiritResult spDestroyMaterial(
    const SpiritContext context,
    SpiritMaterial material);
