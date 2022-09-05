#pragma once
#include <spirit_header.h>

// Create a spirit render context
// Automatically initialize a rendering system
// with all the components

// include components

<<<<<<< HEAD

=======
>>>>>>> devel
typedef struct t_SpiritContextCreateInfo
{

    // window
    SpiritResolution windowSize;
    const char *windowName;
    bool windowFullscreen;

    // device
    bool enableValidation; // should vulkan validation be initialized
<<<<<<< HEAD
    bool powerSaving; // should integrated GPU's be chosen
=======
    bool powerSaving;      // should integrated GPU's be chosen
>>>>>>> devel

} SpiritContextCreateInfo;

struct t_ContextMaterialListNode
{
    SpiritMaterial material;
    LIST_ENTRY(t_ContextMaterialListNode) data;
};

struct t_SpiritContext
{
    SpiritWindow window;

<<<<<<< HEAD
    SpiritDevice     device;
    SpiritSwapchain  swapchain;
=======
    SpiritDevice device;
    SpiritSwapchain swapchain;
>>>>>>> devel

    LIST_HEAD(t_ContextMaterialListHead, t_ContextMaterialListNode) materials;
    u32 materialCount;

    // command buffers
    SpiritCommandBuffer *commandBuffers;
<<<<<<< HEAD
    size_t           commandBufferCount;
=======
    size_t commandBufferCount;
>>>>>>> devel

    SpiritResolution windowSize; // use for UI sizes, stored as screen units
    SpiritResolution screenResolution; // resolution in px, use for

    // sync
    u32 maxImagesInFlight; // the size of all sync objects
    VkSemaphore *imageAvailableSemaphores;
    VkSemaphore *queueCompleteSemaphores;

    // u32 currentFence; // the fence currently rendering
    u32 currentFrame;
};

SpiritContext spCreateContext(SpiritContextCreateInfo *createInfo);

SpiritResult spContextHandleWindowResized(SpiritContext context);

<<<<<<< HEAD

=======
>>>>>>> devel
SpiritResult spContextSubmitFrame(SpiritContext context);

/**
 * @brief add a new material to the context, which will be rendered.
 * The material must be destroyed manually by the user.
 * It can be removed from the context using spContextRemoveMateral.
 *
 * @param context the context the material will be added to. It must be a valid
 * SpiritContext.
 * @param material the material which will be added to the context. It must be
<<<<<<< HEAD
 * kept after the being submitted so that meshes can still be added to the material.
 * It must be a valid SpiritMaterial.
 * @return SpiritResult
 */
SpiritResult spContextAddMaterial(
    SpiritContext context,
    const SpiritMaterial material);
=======
 * kept after the being submitted so that meshes can still be added to the
 * material. It must be a valid SpiritMaterial.
 * @return SpiritResult
 */
SpiritResult
spContextAddMaterial(SpiritContext context, const SpiritMaterial material);
>>>>>>> devel

/**
 * @brief Remove a material from the context that was added using
 * spContextAddMaterial. This will not destroy the material, and it can be added
 * to a different context or added to this context again later.
 *
<<<<<<< HEAD
 * @param context the context the material will be removed from. It must be a valid
 * SpiritContext.
 * @param material the material to remove from the context.
 * @return SpiritResult
 */
SpiritResult spContextRemoveMaterial(
    SpiritContext context,
    const SpiritMaterial material);
=======
 * @param context the context the material will be removed from. It must be a
 * valid SpiritContext.
 * @param material the material to remove from the context.
 * @return SpiritResult
 */
SpiritResult
spContextRemoveMaterial(SpiritContext context, const SpiritMaterial material);
>>>>>>> devel

/**
 * @brief Destroy a spirit context. It will not destroy the meshes associated
 * with those materials.
 *
 * @param context the context to destroy. It must be a valid SpiritContext
 * @return SpiritResult
 */
SpiritResult spDestroyContext(SpiritContext context);
