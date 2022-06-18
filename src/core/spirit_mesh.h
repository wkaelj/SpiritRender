#pragma once
#include <spirit_header.h>

// manage meshes imported into the program
// so that multiple entities can reference the same mesh
//
// It also counts references to each mesh, so that when a mesh is
// no longer referenced it can be destroyed
// 
// Meshes must be checked out and released by the user,
// otherwise they may be held in memory without any references to them,
// or may be destroyed while some references still remain
//
// Kael Johnston June 15 2022

//
// Structures
//

typedef struct t_SpiritMesh
{
    size_t referenceCount;
    size_t vertCount;
    vec3 verts[]; // flex member
} *SpiritMesh;

// a struct used to manage meshes loaded into memory
// each mesh in the meshmanager has its references counted
// and when it has 0 references is automatically released
typedef struct t_SpiritMeshManager
{
    size_t meshCount;
    SpiritMesh meshes[]; // flex member
} *SpiritMeshManager;

// struct containting creation information for the mesh manager
typedef struct t_SpiritMeshManagerCreateInfo
{
    size_t meshCount;
} SpiritMeshManagerCreateInfo;

// a reference to a mesh stored in a mesh manager
// can be obtained and released from
typedef struct t_SpiritMeshReference
{
    size_t index;
    size_t vertCount;
    SpiritMeshManager meshManager;
} SpiritMeshReference;


//
// Functions
//

// create a mesh manager object
extern SpiritMeshManager spCreateMeshManager(
    const SpiritMeshManagerCreateInfo *createInfo);

// add a mesh to a mesh manager
// once a mesh is managed by a mesh manager,
// it should not be freed or modified by other components
extern const SpiritMeshReference spMeshManagerAddMesh(
    SpiritMeshManager *meshManager,
    const SpiritMesh mesh);

// release a reference to a mesh
extern const SpiritResult spReleaseMesh(
    const SpiritMeshReference meshReference);

// checkout a new reference to a mesh
extern const SpiritMeshReference spCheckoutMesh(
    const SpiritMeshReference meshReference);

// destroy a mesh manager
// it will throw a warning if meshes included are still referenced
// but it will still destroy the mesh manager, and all included meshes
extern const SpiritResult spDestroyMeshManager(
    SpiritMeshManager meshManager);
