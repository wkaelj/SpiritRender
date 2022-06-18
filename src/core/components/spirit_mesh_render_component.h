#pragma once
#include <spirit_header.h>
#include <core/spirit_mesh.h>
#include <core/spirit_ecs.h>

// Mesh render component used to pass mesh to a material
// Can be submit to a material, and will be rendered to the screen
// Entity should have a transform component

typedef struct t_SpiritMeshRenderComponent
{
    SpiritMesh mesh;
} SpiritMeshRenderComponent;

typedef struct t_SpiritMeshRenderComponentCreateInfo
{
    SpiritMesh mesh;
} SpiritMeshRenderComponentCreateInfo;


// create a mesh render component
// do not destroy the mesh object referenced by the component,
// it will be destroyed by the component
const SpiritComponentReference spInstantiateMeshRenderComponent(
    const SpiritECS ecs,
    const SpiritMeshRenderComponentCreateInfo *createInfo);
