#pragma once
#include "spirit_window.h"
#include <spirit_header.h>

// Create a spirit render context
// Automatically initialize a rendering system
// with all the components

// include components

typedef struct t_SpiritContextCreateInfo
{

    // window
    SpiritResolution windowSize;
    const char *windowName;
    bool windowFullscreen;

    // device
    bool enableValidation; // should vulkan validation be initialized
    bool powerSaving;      // should integrated GPU's be chosen

} SpiritContextCreateInfo;

struct t_ContextMaterialListNode
{
    SpiritMaterial material;
    LIST_ENTRY(t_ContextMaterialListNode) data;
};

struct t_SpiritContext
{

    /**
     * @brief The window the context renders to
     *
     */
    SpiritWindow window;

    /**
     * @brief the state of the window
     *
     */
    SpiritWindowState windowState;

    /**
     * @brief The device selected by the context
     *
     */
    SpiritDevice device;

    /**
     * @brief The swapchain created by the context
     *
     */
    SpiritSwapchain swapchain;

    /**
     * @brief The list of materials used by the context.
     *
     */
    LIST_HEAD(t_ContextMaterialListHead, t_ContextMaterialListNode) materials;
    u32 materialCount;

    // command buffers
    SpiritCommandBuffer *commandBuffers;
    size_t commandBufferCount;

    SpiritResolution windowSize; // use for UI sizes, stored as screen units
    SpiritResolution screenResolution; // resolution in px, use for

    // sync
    u32 maxImagesInFlight; // the size of all sync objects
    VkSemaphore *imageAvailableSemaphores;
    VkSemaphore *queueCompleteSemaphores;

    // u32 currentFence; // the fence currently rendering
    u32 currentFrame;
};

/**
 * @brief Create a new context. The context included the swapchain, the device,
 * and the sync objects. Materials can be added once it is created.
 *
 * @param createInfo information about the creation of the context, and the
 * objects it contains
 * @return SpiritContext a reference to the created context. This must be
 * destroyed by spDestroyContext when it is no longer used.
 */
SpiritContext spCreateContext(SpiritContextCreateInfo *createInfo);

/**
 * @brief Recreate objects that depend on the window resolution, like the
 * swapchain. Should no longer be called by the app, it is handled by
 * spContextPollEvents.
 *
 * @param context
 * @return SpiritResult
 */
SpiritResult spContextHandleWindowResized(SpiritContext context);

/**
 * @brief Get the state of the of the windows. This is only updated when
 * spContextPollEvents is called.
 *
 * @param context the context containing the window
 * @return SpiritWindowState
 */
SpiritWindowState spContextGetWindowState(const SpiritContext context);

SpiritResult spContextSubmitFrame(SpiritContext context);

SpiritWindowState spContextPollEvents(SpiritContext context);

/**
 * @brief add a new material to the context, which will be rendered.
 * The material must be destroyed manually by the user.
 * It can be removed from the context using spContextRemoveMateral.
 * It also must be removed before it can be destroyed, otherwise the behavoir is
 * undefined.
 *
 * @param context the context the material will be added to. It must be a valid
 * SpiritContext.
 * @param material the material which will be added to the context. It must be
 * kept after the being submitted so that meshes can still be added to the
 * material. It must be a valid SpiritMaterial.
 * @return SpiritResult
 */
SpiritResult
spContextAddMaterial(SpiritContext context, const SpiritMaterial material);

/**
 * @brief Remove a material from the context that was added using
 * spContextAddMaterial. This will not destroy the material, and it can be added
 * to a different context or added to this context again later.
 *
 * @param context the context the material will be removed from. It must be a
 * valid SpiritContext.
 * @param material the material to remove from the context.
 * @return SpiritResult
 */
SpiritResult
spContextRemoveMaterial(SpiritContext context, const SpiritMaterial material);

/**
 * @brief Destroy a spirit context. It will not destroy the meshes associated
 * with those materials.
 *
 * @param context the context to destroy. It must be a valid SpiritContext
 * @return SpiritResult
 */
SpiritResult spDestroyContext(SpiritContext context);
