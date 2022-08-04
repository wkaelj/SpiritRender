#pragma once
#include <spirit_header.h>


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

struct t_MaterialListNode
{
    SpiritMeshReference mesh;
    LIST_ENTRY(t_MaterialListNode) data;
};

struct t_SpiritMaterial
{
    const char *name;
    const char *vertexShader;
    const char *fragmentShader;

    SpiritRenderPass renderPass;
    SpiritPipeline pipeline;

    size_t meshCount;
    LIST_HEAD(t_MaterialListHead, t_MaterialListNode) meshList;
};

SpiritMaterial spCreateMaterial(
    const SpiritContext context,
    const SpiritMaterialCreateInfo *createInfo);

SpiritResult spMaterialUpdate(
    const SpiritContext context, 
    SpiritMaterial material);

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
