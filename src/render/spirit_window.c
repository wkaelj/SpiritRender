

#include "spirit_window.h"
#include <GLFW/glfw3.h>

struct t_SpiritWindow
{
    GLFWwindow *window;
    SpiritResolution windowSize;
    const char *title;
    bool resizing;
};

void window_size_callback(GLFWwindow *window, int width, int height);

SpiritWindow spCreateWindow(SpiritWindowCreateInfo *createInfo)
{

    SpiritWindow window = new_var(struct t_SpiritWindow); // alloc

    const char *glfwError;
    if (glfwInit() != GLFW_TRUE) // alloc
    {
        glfwGetError(&glfwError);
        log_error("GLFW error: %s", glfwError);
        return NULL;
    }

    window->title = createInfo->title;

    GLFWmonitor *windowMonitor = NULL;
    if (createInfo->fullscreen)
        windowMonitor = glfwGetPrimaryMonitor();
    if (createInfo->fullscreen && windowMonitor == NULL)
    {
        glfwGetError(&glfwError);
        log_error("No fullscreen monitor available! GLFW error %s", glfwError);
        createInfo->fullscreen = false;
    }

    if (createInfo->fullscreen)
    {

        // use fullscreen window height
        int fullWidth, fullHeight;
        glfwGetMonitorWorkarea(
            windowMonitor, NULL, NULL, &fullWidth, &fullHeight);

        window->windowSize.w = fullWidth;
        window->windowSize.h = fullHeight;
    }
    else
    {
        window->windowSize = createInfo->windowSize;
    }

    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);

    window->window = glfwCreateWindow( // alloc
        window->windowSize.w,
        window->windowSize.h,
        window->title,
        windowMonitor,
        NULL);

    if (window->window == NULL)
    {
        glfwGetError(&glfwError);
        log_fatal("Failed to create window. GLFW Error: %s", glfwError);
    }

    glfwSetWindowSizeCallback(window->window, &window_size_callback);

    window->resizing = false;

    return window;
}

SpiritResult spDestroyWindow(SpiritWindow window)
{

    glfwDestroyWindow(window->window);
    free(window);
    glfwTerminate();

    log_verbose("Closing window");
    return SPIRIT_SUCCESS;
}

static bool g_glfwWindowWasResized   = false;
static SpiritResolution g_windowSize = {.w = 0, .h = 0};

void window_size_callback(
    GLFWwindow *window __attribute_maybe_unused__, int width, int height)
{
    g_glfwWindowWasResized = true;
    g_windowSize           = (SpiritResolution){.w = width, .h = height};
}

SpiritWindowState spWindowGetState(SpiritWindow window)
{
    const char *description = NULL;

    glfwPollEvents();
    if (glfwGetError(&description))
    {
        log_error("GLFW error: %s", description);
        return SPIRIT_WINDOW_CLOSED;
    }

    if (glfwWindowShouldClose(window->window))
    {
        if (glfwGetError(&description))
        {
            log_error("GLFW error: %s", description);
            return SPIRIT_WINDOW_CLOSED;
        }
        return SPIRIT_WINDOW_CLOSED;
    }

    // the window is being resized
    if (g_glfwWindowWasResized)
    {
        g_glfwWindowWasResized = false;
        window->windowSize     = g_windowSize;
        window->resizing       = true;
        g_windowSize           = (SpiritResolution){};
        return SPIRIT_WINDOW_RESIZING;
    }

    window->resizing = false;

    return SPIRIT_WINDOW_NORMAL;
}

SpiritResult spResizeWindow(SpiritWindow window, uint32_t w, uint32_t h)
{

    if (window->window == NULL)
    {
        log_error("spResizeWindow: window passed is NULL");
    }

    glfwSetWindowSize(window->window, w, h);

    return SPIRIT_SUCCESS;
}

SpiritResolution spWindowGetPixelSize(SpiritWindow window)
{

    const char *glfwError;

    SpiritResolution res = {};
    int w, h;
    glfwGetFramebufferSize(window->window, &w, &h);
    if (glfwGetError(&glfwError) != GLFW_NO_ERROR)
    {
        log_error("Failed to get framebuffer size. GLFW error: %s", glfwError);
        return res;
    }
    res.w = w, res.h = h;

    return res;
}

SpiritResolution spWindowGetSize(SpiritWindow window)
{
    return window->windowSize;
}

VkSurfaceKHR spWindowGetSurface(SpiritWindow window, VkInstance instance)
{

    VkSurfaceKHR surface = NULL;

    if (glfwCreateWindowSurface(instance, window->window, NULL, &surface))
        log_fatal("Failed to create window surface");
    db_assert_msg(surface, "Surface cannot be NULL");
    return surface;
}

SpiritWindowExtensions spWindowGetExtensions(SpiritWindow window)
{

    SpiritWindowExtensions out = {};

    out.names = glfwGetRequiredInstanceExtensions(&out.count);

    return out;
}
