#pragma once
#include <spirit_header.h>

// Create a window, with a given size
//
//
// Kael Johnston Feb 26 2022

//
// types
//

typedef struct t_SpiritWindowCreateInfo
{
    SpiritResolution windowSize; // size of window in screen units
    const char *title;
    bool fullscreen;
} SpiritWindowCreateInfo;

typedef struct t_SpiritWindowExtensions
{
    u32          count;
    const char **names;
} SpiritWindowExtensions;

typedef enum e_SpiritWindowState
{
    SPIRIT_WINDOW_NORMAL = 0,
    SPIRIT_WINDOW_RESIZED,
    SPIRIT_WINDOW_RESIZING,
    SPIRIT_WINDOW_CLOSED
} SpiritWindowState;


/**
 * @brief Create a window for the current platform
 *
 * @param createInfo a pointer to a SpiritWindowCreateInfo struct, with info
 * on how to create the window
 * @return SpiritWindow
 */
SpiritWindow spCreateWindow(SpiritWindowCreateInfo *createInfo) SPIRIT_NONULL(1);

/**
 * @brief Close and destroy a SpiritWindow
 *
 * @param window the window to close
 * @return SpiritResult
 */
SpiritResult spDestroyWindow(SpiritWindow window) SPIRIT_NONULL(1);

/**
 * @brief Update a window and check if it has been closed or resized by the
 * user.
 *
 * @param window
 * @return SpiritWindowState
 */
SpiritWindowState spWindowGetState(SpiritWindow window) SPIRIT_NONULL(1);

/**
 * @brief resize the window
 *
 * @param window
 * @param w the window width in screen units
 * @param h the window height in screen units
 * @return SpiritResult
 */
SpiritResult spResizeWindow(SpiritWindow window, uint32_t w, uint32_t h)
    SPIRIT_NONULL(1);

/**
 * @brief Get the window size in pixels, not screen units.
 *
 * @param window the window
 * @return SpiritResolution the window size in pixels
 */
SpiritResolution spWindowGetPixelSize(SpiritWindow window) SPIRIT_NONULL(1);

/**
 * @brief Get the size of the window in screen units
 *
 * @param window
 * @return SpiritResolution
 */
SpiritResolution spWindowGetSize(SpiritWindow window) SPIRIT_NONULL(1);

/**
 * @brief Get the GPU extensions required to support
 * the window
 *
 * @param window
 * @return SpiritWindowExtensions (const char **)
 */
SpiritWindowExtensions spWindowGetExtensions(SpiritWindow window)
    SPIRIT_NONULL(1);

/**
 * @brief Get a window surface for vulkan to render too.
 *
 * @param window
 * @param instance a vulkan instance
 * @return VkSurfaceKHR
 */
VkSurfaceKHR spWindowGetSurface(SpiritWindow window, VkInstance instance)
    SPIRIT_NONULL(1, 2);
