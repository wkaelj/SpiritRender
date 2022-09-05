#include "spirit_mesh.h"

#include "spirit_command_buffer.h"
<<<<<<< HEAD
#include "spirit_device.h"
#include "spirit_context.h"
=======
#include "spirit_context.h"
#include "spirit_device.h"
>>>>>>> devel
//
// Public Functions
//
//

<<<<<<< HEAD
SpiritMesh spCreateMesh(const SpiritContext context, const SpiritMeshCreateInfo *createInfo)
{

    SpiritMesh mesh = new_flex_array(struct t_SpiritMesh, Vertex, createInfo->vertCount);
=======
SpiritMesh spCreateMesh(
    const SpiritContext context, const SpiritMeshCreateInfo *createInfo)
{

    SpiritMesh mesh =
        new_flex_array(struct t_SpiritMesh, Vertex, createInfo->vertCount);
    mesh->vertCount = createInfo->vertCount;
>>>>>>> devel

    // process vertex data
    size_t dataSize = sizeof(Vertex) * createInfo->vertCount;

    for (size_t i = 0; i < createInfo->vertCount; i++)
    {
<<<<<<< HEAD
        mesh->verts[i] = (Vertex) { 0 };
=======
        mesh->verts[i] = (Vertex){0};
>>>>>>> devel
        glm_vec3_copy(createInfo->verts[i], mesh->verts[i].position);
    }

    // obtain memory from device
    VkBuffer hostBuffer;
    VkDeviceMemory hostBufferMemory;
    VkDeviceSize bufferSize = dataSize;
<<<<<<< HEAD
    if(spDeviceCreateBuffer(
        context->device,
        bufferSize,
        VK_BUFFER_USAGE_VERTEX_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
        VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT,
        &hostBuffer,
        &hostBufferMemory))
=======
    if (spDeviceCreateBuffer(
            context->device,
            bufferSize,
            VK_BUFFER_USAGE_VERTEX_BUFFER_BIT |
                VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
            VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT,
            &hostBuffer,
            &hostBufferMemory))
>>>>>>> devel
    {
        log_error("Failed to create mesh");
        return NULL;
    }

    // copy memory into data
    Vertex *data;
<<<<<<< HEAD
    vkMapMemory(context->device->device, hostBufferMemory, 0, bufferSize, 0, (void**) &data);
=======
    vkMapMemory(
        context->device->device,
        hostBufferMemory,
        0,
        bufferSize,
        0,
        (void **)&data);
>>>>>>> devel
    memcpy(data, mesh->verts, dataSize);
    vkUnmapMemory(context->device->device, hostBufferMemory);

    VkBuffer localBuffer;
    VkDeviceMemory localBufferMemory;
<<<<<<< HEAD
        if(spDeviceCreateBuffer(
        context->device,
        bufferSize,
        VK_BUFFER_USAGE_VERTEX_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT,
        VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
        &localBuffer,
        &localBufferMemory))
=======
    if (spDeviceCreateBuffer(
            context->device,
            bufferSize,
            VK_BUFFER_USAGE_VERTEX_BUFFER_BIT |
                VK_BUFFER_USAGE_TRANSFER_DST_BIT,
            VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
            &localBuffer,
            &localBufferMemory))
>>>>>>> devel
    {
        log_error("Failed to create mesh");
        return NULL;
    }

    // use a single-use command buffer to copy the data
<<<<<<< HEAD
    SpiritCommandBuffer buf = spCreateCommandBufferAndBeginSingleUse(context->device);

    VkBufferCopy copyData = {
        .dstOffset = 0,
        .srcOffset = 0,
        .size = bufferSize
    };
=======
    SpiritCommandBuffer buf =
        spCreateCommandBufferAndBeginSingleUse(context->device);

    VkBufferCopy copyData = {
        .dstOffset = 0, .srcOffset = 0, .size = bufferSize};
>>>>>>> devel

    vkCmdCopyBuffer(buf->handle, hostBuffer, localBuffer, 1, &copyData);

    if (spCommandBufferSubmitSingleUse(context->device, buf))
    {
        log_fatal("Failed to create mesh");
        return NULL;
    }

    spCommandBufferWait(context->device, buf, UINT32_MAX);

    spDestroyCommandBuffer(context->device, buf);

    // destroy temp buffers
<<<<<<< HEAD
    vkDestroyBuffer(
        context->device->device,
        hostBuffer,
        NULL);
    vkFreeMemory(
        context->device->device,
        hostBufferMemory,
        NULL);

    // update mesh t reference vertex data
    mesh->vertexBuffer = localBuffer;
=======
    vkDestroyBuffer(context->device->device, hostBuffer, NULL);
    vkFreeMemory(context->device->device, hostBufferMemory, NULL);

    // update mesh t reference vertex data
    mesh->vertexBuffer      = localBuffer;
>>>>>>> devel
    mesh->vetexBufferMemory = localBufferMemory;

    return mesh;
}

SpiritMeshManager spCreateMeshManager(
    const SpiritContext context,
    __attribute__((unused)) const SpiritMeshManagerCreateInfo *createInfo)
{
    SpiritMeshManager meshManager = new_var(struct t_SpiritMeshManager);
<<<<<<< HEAD
    meshManager->meshCount = 0;
=======
    meshManager->meshCount        = 0;
>>>>>>> devel
    LIST_INIT(&meshManager->meshes);

    meshManager->contextReference = context;

    return meshManager;
}

<<<<<<< HEAD
SpiritMeshReference spMeshManagerAddMesh(
    SpiritMeshManager manager,
    SpiritMesh mesh)
=======
SpiritMeshReference
spMeshManagerAddMesh(SpiritMeshManager manager, SpiritMesh mesh)
>>>>>>> devel
{

    db_assert_msg(mesh && manager, "Must have both a mesh and a manager");

    if (!mesh || !manager)
    {
<<<<<<< HEAD
        return (SpiritMeshReference) {NULL, 0, NULL};
    }

    struct t_MeshListNode *newNode = new_var(struct t_MeshListNode);
    newNode->mesh = mesh;
    newNode->referenceCount = 0;

    LIST_INSERT_HEAD(&manager->meshes, newNode, data);
    SpiritMeshReference ref = {};
    ref.meshManager = manager;
    ref.node = newNode;
    ref.vertCount = mesh->vertCount;
=======
        return (SpiritMeshReference){NULL, 0, NULL};
    }

    struct t_MeshListNode *newNode = new_var(struct t_MeshListNode);
    newNode->mesh                  = mesh;
    newNode->referenceCount        = 0;

    LIST_INSERT_HEAD(&manager->meshes, newNode, data);
    SpiritMeshReference ref = {};
    ref.meshManager         = manager;
    ref.node                = newNode;
    ref.vertCount           = mesh->vertCount;
>>>>>>> devel

    return spCheckoutMesh(ref);
}

<<<<<<< HEAD
SpiritMesh spMeshManagerAccessMesh(
    const SpiritMeshReference ref)
=======
SpiritMesh spMeshManagerAccessMesh(const SpiritMeshReference ref)
>>>>>>> devel
{
    return ref.node->mesh;
}

// checkout a new reference to a mesh
SpiritMeshReference spCheckoutMesh(const SpiritMeshReference meshReference)
{
<<<<<<< HEAD
     meshReference.node->referenceCount++;
     return meshReference;
}

// release a reference to a mesh
SpiritResult spReleaseMesh(
    const SpiritMeshReference meshReference)
{

    // check to ensure meshmanager is valid
    if(meshReference.node
        && meshReference.node->mesh
        && meshReference.vertCount
        && meshReference.meshManager); else { return SPIRIT_FAILURE; }
=======
    meshReference.node->referenceCount++;
    return meshReference;
}

// release a reference to a mesh
SpiritResult spReleaseMesh(const SpiritMeshReference meshReference)
{

    // check to ensure meshmanager is valid
    if (meshReference.node && meshReference.node->mesh &&
        meshReference.vertCount && meshReference.meshManager)
        ;
    else
    {
        return SPIRIT_FAILURE;
    }
>>>>>>> devel

    // reduce reference count, and if no more references free mesh
    if (--meshReference.node->referenceCount <= 0)
    {
        LIST_REMOVE(meshReference.node, data);
        spDestroyMesh(
            meshReference.meshManager->contextReference,
            meshReference.node->mesh);
        free(meshReference.node);
        meshReference.meshManager->meshCount--;
    }

    return SPIRIT_SUCCESS;
}

SpiritResult spDestroyMesh(const SpiritContext context, SpiritMesh mesh)
{
<<<<<<< HEAD
    vkDestroyBuffer(
        context->device->device,
        mesh->vertexBuffer,
        NULL);
    vkFreeMemory(
        context->device->device,
        mesh->vetexBufferMemory,
        NULL);
=======
    vkDestroyBuffer(context->device->device, mesh->vertexBuffer, NULL);
    vkFreeMemory(context->device->device, mesh->vetexBufferMemory, NULL);
>>>>>>> devel

    free(mesh);

    return SPIRIT_SUCCESS;
}

<<<<<<< HEAD
SpiritResult spDestroyMeshManager(
    const SpiritContext context,
    SpiritMeshManager meshManager)
=======
SpiritResult
spDestroyMeshManager(const SpiritContext context, SpiritMeshManager meshManager)
>>>>>>> devel
{

    struct t_MeshListNode *cn = NULL;
    LIST_FOREACH(cn, &meshManager->meshes, data)
    {
        spDestroyMesh(context, cn->mesh);
    }

<<<<<<< HEAD
    while(!LIST_EMPTY(&meshManager->meshes))
=======
#ifdef DEBUG
    u32 deletedMeshCount = 0;
#endif
    while (!LIST_EMPTY(&meshManager->meshes))
>>>>>>> devel
    {
        struct t_MeshListNode *first = LIST_FIRST(&meshManager->meshes);
        LIST_REMOVE(first, data);
        free(first);
<<<<<<< HEAD
    }

=======

#ifdef DEBUG
        ++deletedMeshCount;
#endif
    }

#ifdef DEBUG
    log_debug("Deleted %u meshes", deletedMeshCount);
#endif

>>>>>>> devel
    free(meshManager);
    return SPIRIT_SUCCESS;
}

<<<<<<< HEAD
SPIRIT_INLINE VkVertexInputAttributeDescription spMeshGetAttributeDescription(void)
{
    return (VkVertexInputAttributeDescription) {
=======
SPIRIT_INLINE VkVertexInputAttributeDescription
spMeshGetAttributeDescription(void)
{
    return (VkVertexInputAttributeDescription){
>>>>>>> devel
        0, 0, VK_FORMAT_R32G32B32_SFLOAT, 0};
}

SPIRIT_INLINE VkVertexInputBindingDescription spMeshGetBindingDescription(void)
{
<<<<<<< HEAD
    return (VkVertexInputBindingDescription) {
        0,
        sizeof(Vertex),
        VK_VERTEX_INPUT_RATE_VERTEX };
=======
    return (VkVertexInputBindingDescription){
        0, sizeof(Vertex), VK_VERTEX_INPUT_RATE_VERTEX};
>>>>>>> devel
}