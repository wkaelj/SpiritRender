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
struct t_SpiritMeshManager
{
    size_t meshCount;
    LIST_HEAD(t_MeshList, t_MeshListNode) meshes;
};

// struct containting creation information for the mesh manager
typedef struct t_SpiritMeshManagerCreateInfo
{
    // empty, might be used later though
    int val;
} SpiritMeshManagerCreateInfo;




//
// Functions
//

// create a mesh that can be added to a mesh manager
// converts vector3 mesh positions to vertex structs
// and creates vertex buffers
extern SpiritMesh spCreateMesh(
    const SpiritContext context,
    const SpiritMeshCreateInfo *createInfo);

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

const VkVertexInputAttributeDescription spMeshGetAttributeDescription(void);

const VkVertexInputBindingDescription spMeshGetBindingDescription(void);


