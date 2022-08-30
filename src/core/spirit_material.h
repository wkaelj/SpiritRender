/**
 * @file spirit_material.h
 * @author Kael Johnston (you@domain.com)
 * @brief Create materials that can be assigned meshes. Each material must be
 * added to a SpiritContext, and it will then be rendered. Meshes must be added
 * to a material each frame, using a call to spMaterialAddMesh, with the meshes
 * push constants for that frame. The materials are not destroyed by the
 * context, and must be destroyed by the user using the handle returned by
 * spCreateMaterial.
 * @version 0.1
 * @date 2022-08-28
 *
 * @copyright Copyright (c) 2022
 *
 */

#pragma once
#include <spirit_header.h>

/**
 * @brief Information to create a material
 *
 */
typedef struct t_SpiritMaterialCreateInfo
{
    const char *name;
    const char *vertexShader;
    const char *fragmentShader;

} SpiritMaterialCreateInfo;

struct t_MaterialListNode
{
    SpiritMeshReference mesh;
    LIST_ENTRY(t_MaterialListNode) data;
};

/**
 * @brief Store data needed to render a material
 *
 */
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

/**
 * @brief Create a new material with the properties defined in createInfo
 *
 * @param context the context used to render the material
 * @param createInfo the information used to create the material
 * @return SpiritMaterial the handle to the material
 */
SpiritMaterial spCreateMaterial(
    const SpiritContext context, const SpiritMaterialCreateInfo *createInfo);

/**
 * @brief Update a material to have a relevant window size, and to know all the
 * information it needs about a window
 *
 * @param context
 * @param material
 * @return SpiritResult
 */
SpiritResult
spMaterialUpdate(const SpiritContext context, SpiritMaterial material);

/**
 * @brief Add a mesh to the material, which will be rendered in the next frame.
 * A mesh will only be rendered when spContextSubmitCommands is called, and then
 * it will be removed from the material. The mesh will also only be rendered
 * when.
 *
 * @param material
 * @param meshRef
 * @return SpiritResult
 */
SpiritResult spMaterialAddMesh(
    const SpiritMaterial material, const SpiritMeshReference meshRef);

/**
 * @brief Not to be used by the user, spMaterialRecordCommands is used by the
 * spContextRecordCommands function to draw its materials.
 *
 * @param context
 * @param material
 * @param imageIndex
 * @return SpiritResult
 */
SpiritResult spMaterialRecordCommands(
    const SpiritContext context, SpiritMaterial material, const u32 imageIndex);

/**
 * @brief Destroy a material. This will not destroy any meshes added to the
 * material. be sure to remove the material from the context you added it to,
 * otherwise there will be a segmentation fault.
 *
 * @param context
 * @param material
 * @return SpiritResult
 */
SpiritResult
spDestroyMaterial(const SpiritContext context, SpiritMaterial material);
