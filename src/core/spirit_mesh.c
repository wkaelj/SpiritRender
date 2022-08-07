#include "spirit_mesh.h"

#include "spirit_device.h"
#include "spirit_context.h"
//
// Public Functions
//
//

SpiritMesh spCreateMesh(const SpiritContext context, const SpiritMeshCreateInfo *createInfo)
{
    
    SpiritMesh mesh = new_flex_array(struct t_SpiritMesh, Vertex, createInfo->vertCount);

    // process vertex data
    size_t dataSize = sizeof(Vertex) * createInfo->vertCount;

    for (size_t i = 0; i < createInfo->vertCount; i++)
    {
        mesh->verts[i] = (Vertex) { 0 };
        glm_vec3_copy(createInfo->verts[i], mesh->verts[i].position);
    }

    // obtain memory from device
    VkBuffer vertBuffer;
    VkDeviceMemory vertBufferMemory;
    VkDeviceSize bufferSize = dataSize;
    if(spDeviceCreateBuffer(
        context->device,
        bufferSize,
        VK_BUFFER_USAGE_VERTEX_BUFFER_BIT,
        VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT,
        &vertBuffer,
        &vertBufferMemory))
    {
        return NULL;
    }

    // copy memory into data
    Vertex *data;
    vkMapMemory(context->device->device, vertBufferMemory, 0, bufferSize, 0, (void**) &data);
    memcpy(data, mesh->verts, dataSize);
    vkUnmapMemory(context->device->device, vertBufferMemory);

    // update mesh t reference vertex data
    mesh->vertexBuffer = vertBuffer;
    mesh->vetexBufferMemory = vertBufferMemory;

    return mesh;
}

SpiritMeshManager spCreateMeshManager(
    const SpiritContext context, 
    const SpiritMeshManagerCreateInfo *createInfo)
{
    SpiritMeshManager meshManager = new_var(struct t_SpiritMeshManager);
    meshManager->meshCount = 0;
    LIST_INIT(&meshManager->meshes);

    meshManager->contextReference = context;

    return meshManager;
}

const SpiritMeshReference spMeshManagerAddMesh(
    SpiritMeshManager manager,
    SpiritMesh mesh)
{
    struct t_MeshListNode *newNode = new_var(struct t_MeshListNode);
    newNode->mesh = mesh;
    newNode->referenceCount = 0;

    LIST_INSERT_HEAD(&manager->meshes, newNode, data);
    SpiritMeshReference ref = {};
    ref.meshManager = manager;
    ref.node = newNode;
    ref.vertCount = mesh->vertCount;

    return spCheckoutMesh(ref);
}

const SpiritMesh spMeshManagerAccessMesh(
    const SpiritMeshReference ref)
{
    return ref.node->mesh;
}

// checkout a new reference to a mesh
const SpiritMeshReference spCheckoutMesh(const SpiritMeshReference meshReference)
{
     meshReference.node->referenceCount++;
     return meshReference;
}

// release a reference to a mesh
const SpiritResult spReleaseMesh(
    const SpiritMeshReference meshReference)
{

    // check to ensure meshmanager is valid
    if(meshReference.node 
        && meshReference.node->mesh
        && meshReference.vertCount 
        && meshReference.meshManager); else { return SPIRIT_FAILURE; }

    // reduce reference count, and if no more references free mesh
    if (--meshReference.node->referenceCount <= 0)
    {
        LIST_REMOVE(meshReference.node, data);
        spDestroyMesh(
            meshReference.meshManager->contextReference, 
            meshReference.node->mesh);
        free(meshReference.node->mesh);
        free(meshReference.node);
        meshReference.meshManager->meshCount--;
    }

    return SPIRIT_SUCCESS;
}

SpiritResult spDestroyMesh(const SpiritContext context, SpiritMesh mesh)
{
    vkDestroyBuffer(
        context->device->device,
        mesh->vertexBuffer,
        NULL);
    vkFreeMemory(
        context->device->device,
        mesh->vetexBufferMemory,
        NULL);

    free(mesh);

    return SPIRIT_SUCCESS;
}

SpiritResult spDestroyMeshManager(
    const SpiritContext context,
    SpiritMeshManager meshManager)
{
    
    struct t_MeshListNode *cn = NULL;
    LIST_FOREACH(cn, &meshManager->meshes, data)
    {
        spDestroyMesh(context, cn->mesh);
    }

    while(!LIST_EMPTY(&meshManager->meshes))
    {
        struct t_MeshListNode *first = LIST_FIRST(&meshManager->meshes);
        LIST_REMOVE(first, data);
        free(first);
    }

    free(meshManager);
    return SPIRIT_SUCCESS;
}

const VkVertexInputAttributeDescription spMeshGetAttributeDescription(void)
{
    return (VkVertexInputAttributeDescription) {
        0, 0, VK_FORMAT_R32G32B32_SFLOAT, 0};
}

const VkVertexInputBindingDescription spMeshGetBindingDescription(void)
{
    return (VkVertexInputBindingDescription) {
        0, 
        sizeof(Vertex), 
        VK_VERTEX_INPUT_RATE_VERTEX };
}


