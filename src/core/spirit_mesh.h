#pragma once
#include <spirit_header.h>
#include "spirit_device.h"
#include "spirit_context.h"

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

// mesh data

typedef struct t_Vertex
{
    vec3 position;
} Vertex;

typedef struct t_SpiritMeshCreateInfo
{
    vec3 *verts;
    size_t vertCount;

} SpiritMeshCreateInfo;

typedef struct t_SpiritMesh
{
    size_t vertCount;
    VkBuffer vertexBuffer;
    VkDeviceMemory vetexBufferMemory;

    Vertex verts[]; // flex member
} *SpiritMesh;

typedef struct t_SpiritMaterialMeshList *t_SpiritMaterialMeshList;

struct t_MeshListNode
{
    size_t referenceCount;
    SpiritMesh mesh;
    LIST_ENTRY(t_MeshListNode) data; // list data
};

// mesh manager info

// a struct used to manage meshes loaded into memory
// each mesh in the meshmanager has its references counted
// and when it has 0 references is automatically released
typedef struct t_SpiritMeshManager
{
    size_t meshCount;
    LIST_HEAD(t_MeshList, t_MeshListNode) meshes;
} *SpiritMeshManager;

// struct containting creation information for the mesh manager
typedef struct t_SpiritMeshManagerCreateInfo
{
    // empty, might be used later though
} SpiritMeshManagerCreateInfo;

// a reference to a mesh stored in a mesh manager
// can be obtained and released from
typedef struct t_SpiritMeshReference
{
    struct t_MeshListNode *node;
    size_t vertCount;
    SpiritMeshManager meshManager;
} SpiritMeshReference;


//
// Functions
//

// create a mesh that can be added to a mesh manager
// converts vector3 mesh positions to vertex structs
// and creates vertex buffers
extern SpiritMesh spCreateMesh(const SpiritDevice device, const SpiritMeshCreateInfo *createInfo);

// you really shouldnt ever need to call this, but you do you
extern SpiritResult spDestroyMesh(const SpiritContext context, SpiritMesh mesh);

// create a mesh manager object
extern SpiritMeshManager spCreateMeshManager(const SpiritMeshManagerCreateInfo *createInfo);

// add a mesh to a mesh manager
// once a mesh is managed by a mesh manager,
// it should not be freed or modified by other components
extern const SpiritMeshReference spMeshManagerAddMesh(
    SpiritMeshManager manager,
    SpiritMesh mesh);

// release a reference to a mesh
extern const SpiritResult spReleaseMesh(const SpiritMeshReference meshReference);

// checkout a new reference to a mesh
extern const SpiritMeshReference spCheckoutMesh(const SpiritMeshReference meshReference);

// access the mesh referenced by a meshreference
extern const SpiritMesh spMeshManagerAccessMesh(const SpiritMeshReference ref);

// destroy a mesh manager
// it will throw a warning if meshes included are still referenced
// but it will still destroy the mesh manager, and all included meshes
extern SpiritResult spDestroyMeshManager(
    const SpiritContext context,
    SpiritMeshManager meshManager);

static inline const VkVertexInputAttributeDescription spMeshGetAttributeDescription(void)
{
    return (VkVertexInputAttributeDescription) {
        0, 0, VK_FORMAT_R32G32B32_SFLOAT, 0};
}

static inline const VkVertexInputBindingDescription spMeshGetBindingDescription(void)
{
    return (VkVertexInputBindingDescription) {
        0, 
        sizeof(Vertex), 
        VK_VERTEX_INPUT_RATE_VERTEX };
}

