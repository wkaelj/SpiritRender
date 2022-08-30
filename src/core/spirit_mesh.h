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
} * SpiritMesh;

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
    SpiritContext contextReference;
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

/**
 * @brief Create a mesh that can be drawn by a material. The mesh cannot be used
 * until it is added to a mesh manager, which will allow you to reference the
 * material elsewhere in the program.
 *
 * @param context the context that the mesh will be used with
 * @param createInfo information to create the mesh
 * @return SpiritMesh a mesh object, which must be added to a mesh manager
 */
extern SpiritMesh spCreateMesh(
    const SpiritContext context, const SpiritMeshCreateInfo *createInfo);

/**
 * @brief Destroy a mesh object. This function should rarely be used, as this is
 * done automatically by the mesh manager. It may be useful in failure cases
 * where the mesh has been created, but not added to the mesh manager.
 *
 * @param context
 * @param mesh
 * @return SpiritResult
 */
extern SpiritResult spDestroyMesh(const SpiritContext context, SpiritMesh mesh);

/**
 * @brief Create a mesh manager. The mesh manager stores meshes so they can
 * be used by different materials, with different transforms. The mesh manager
 * counts references to each mesh it contains and when a mesh is no longer
 * referenced it is unloaded.
 *
 * @param context
 * @param createInfo
 * @return SpiritMeshManager
 */
extern SpiritMeshManager spCreateMeshManager(
    const SpiritContext context, const SpiritMeshManagerCreateInfo *createInfo);

/**
 * @brief Add a mesh to the mesh manager. Once it is part of the mesh manager,
 * it should no longer be modified by the user. For example, destroying a mesh
 * used by a mesh manager will result in a segmentation fault.
 *
 * @param manager
 * @param mesh
 * @return SpiritMeshReference
 */
extern SpiritMeshReference
spMeshManagerAddMesh(SpiritMeshManager manager, SpiritMesh mesh);

/**
 * @brief Release a reference to a mesh. This must be done when a meshReference
 * is going out of scope, or will no longer be used. This lets the mesh mangager
 * know that this mesh reference is no longer used, and if it was the last one
 * it can destroy the mesh. If you do not do this, it will result in memory
 * leaks.
 *
 * @param meshReference
 * @return SpiritResult
 */
extern SpiritResult spReleaseMesh(const SpiritMeshReference meshReference);

/**
 * @brief Get an additional mesh reference from a mesh reference. This function
 * should be used if you are going to have two references to the same mesh,
 * or are going to store the same mesh reference in two places, so that if you
 * release one of the references it does not destroy the mesh. The function
 * tells the mesh manager that there is an additional reference to the mesh in
 * question
 *
 * @param meshReference a reference to the mesh
 * @return SpiritMeshReference a new reference to the mesh, which is safe to use
 */
extern SpiritMeshReference
spCheckoutMesh(const SpiritMeshReference meshReference);

/**
 * @brief Access the mesh object referened by a mesh reference.
 *
 * @param ref
 * @return SpiritMesh
 */
extern SpiritMesh spMeshManagerAccessMesh(const SpiritMeshReference ref);

/**
 * @brief Destroy a mesh manager and all the contained meshes. If any meshes are
 * still in use when it is destroyed, it will give a warning, but still destroy
 * the meshes.
 *
 * @param context
 * @param meshManager
 * @return SpiritResult
 */
extern SpiritResult spDestroyMeshManager(
    const SpiritContext context, SpiritMeshManager meshManager);

VkVertexInputAttributeDescription spMeshGetAttributeDescription(void);
VkVertexInputBindingDescription spMeshGetBindingDescription(void);
