#include "spirit_mesh.h"

#include "spirit_command_buffer.h"
#include "spirit_context.h"
#include "spirit_device.h"
//
// Public Functions
//
//

SpiritMesh spCreateMesh(
    const SpiritContext context, const SpiritMeshCreateInfo *createInfo)
{

    SpiritMesh mesh =
        new_flex_array(struct t_SpiritMesh, Vertex, createInfo->vertCount);
    mesh->vertCount = createInfo->vertCount;

    // process vertex data
    size_t dataSize = sizeof(Vertex) * createInfo->vertCount;

    for (size_t i = 0; i < createInfo->vertCount; i++)
    {
        mesh->verts[i] = (Vertex){0};
        glm_vec3_copy(createInfo->verts[i], mesh->verts[i].position);
    }

    // obtain memory from device
    VkBuffer hostBuffer;
    VkDeviceMemory hostBufferMemory;
    VkDeviceSize bufferSize = dataSize;
    if (spDeviceCreateBuffer(
            context->device,
            bufferSize,
            VK_BUFFER_USAGE_VERTEX_BUFFER_BIT |
                VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
            VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT,
            &hostBuffer,
            &hostBufferMemory))
    {
        log_error("Failed to create mesh");
        return NULL;
    }

    // copy memory into data
    Vertex *data;
    vkMapMemory(
        context->device->device,
        hostBufferMemory,
        0,
        bufferSize,
        0,
        (void **)&data);
    memcpy(data, mesh->verts, dataSize);
    vkUnmapMemory(context->device->device, hostBufferMemory);

    VkBuffer localBuffer;
    VkDeviceMemory localBufferMemory;
    if (spDeviceCreateBuffer(
            context->device,
            bufferSize,
            VK_BUFFER_USAGE_VERTEX_BUFFER_BIT |
                VK_BUFFER_USAGE_TRANSFER_DST_BIT,
            VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
            &localBuffer,
            &localBufferMemory))
    {
        log_error("Failed to create mesh");
        return NULL;
    }

    // use a single-use command buffer to copy the data
    SpiritCommandBuffer buf =
        spCreateCommandBufferAndBeginSingleUse(context->device);

    VkBufferCopy copyData = {
        .dstOffset = 0, .srcOffset = 0, .size = bufferSize};

    vkCmdCopyBuffer(buf->handle, hostBuffer, localBuffer, 1, &copyData);

    if (spCommandBufferSubmitSingleUse(context->device, buf))
    {
        log_fatal("Failed to create mesh");
        return NULL;
    }

    spCommandBufferWait(context->device, buf, UINT32_MAX);

    spDestroyCommandBuffer(context->device, buf);

    // destroy temp buffers
    vkDestroyBuffer(context->device->device, hostBuffer, NULL);
    vkFreeMemory(context->device->device, hostBufferMemory, NULL);

    // update mesh t reference vertex data
    mesh->vertexBuffer      = localBuffer;
    mesh->vetexBufferMemory = localBufferMemory;

    return mesh;
}

SpiritMeshManager spCreateMeshManager(
    const SpiritContext context,
    __attribute__((unused)) const SpiritMeshManagerCreateInfo *createInfo)
{
    SpiritMeshManager meshManager = new_var(struct t_SpiritMeshManager);
    meshManager->meshCount        = 0;
    LIST_INIT(&meshManager->meshes);

    meshManager->contextReference = context;

    return meshManager;
}

SpiritMeshReference
spMeshManagerAddMesh(SpiritMeshManager manager, SpiritMesh mesh)
{

    db_assert_msg(mesh && manager, "Must have both a mesh and a manager");

    if (!mesh || !manager)
    {
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

    return spCheckoutMesh(ref);
}

SpiritMesh spMeshManagerAccessMesh(const SpiritMeshReference ref)
{
    return ref.node->mesh;
}

// checkout a new reference to a mesh
SpiritMeshReference spCheckoutMesh(const SpiritMeshReference meshReference)
{
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
    vkDestroyBuffer(context->device->device, mesh->vertexBuffer, NULL);
    vkFreeMemory(context->device->device, mesh->vetexBufferMemory, NULL);

    free(mesh);

    return SPIRIT_SUCCESS;
}

SpiritResult
spDestroyMeshManager(const SpiritContext context, SpiritMeshManager meshManager)
{

    struct t_MeshListNode *cn = NULL;
    LIST_FOREACH(cn, &meshManager->meshes, data)
    {
        spDestroyMesh(context, cn->mesh);
    }

#ifdef DEBUG
    u32 deletedMeshCount = 0;
#endif
    while (!LIST_EMPTY(&meshManager->meshes))
    {
        struct t_MeshListNode *first = LIST_FIRST(&meshManager->meshes);
        LIST_REMOVE(first, data);
        free(first);

#ifdef DEBUG
        ++deletedMeshCount;
#endif
    }

#ifdef DEBUG
    log_debug("Deleted %u meshes", deletedMeshCount);
#endif

    free(meshManager);
    return SPIRIT_SUCCESS;
}

SPIRIT_INLINE VkVertexInputAttributeDescription
spMeshGetAttributeDescription(void)
{
    return (VkVertexInputAttributeDescription){
        0, 0, VK_FORMAT_R32G32B32_SFLOAT, 0};
}

SPIRIT_INLINE VkVertexInputBindingDescription spMeshGetBindingDescription(void)
{
    return (VkVertexInputBindingDescription){
        0, sizeof(Vertex), VK_VERTEX_INPUT_RATE_VERTEX};
}