

#include "spirit_window.h"
#include <GLFW/glfw3.h>

<<<<<<< HEAD
struct t_SpiritWindow {
=======
struct t_SpiritWindow
{
>>>>>>> devel
    GLFWwindow *window;
    SpiritResolution windowSize;
    const char *title;
    bool resizing;
};

<<<<<<< HEAD
void window_size_callback(GLFWwindow* window, int width, int height);

SpiritWindow spCreateWindow (SpiritWindowCreateInfo *createInfo) {

    SpiritWindow window = new_var(struct t_SpiritWindow);
=======
void window_size_callback(GLFWwindow *window, int width, int height);

SpiritWindow spCreateWindow(SpiritWindowCreateInfo *createInfo)
{

    SpiritWindow window = new_var(struct t_SpiritWindow); // alloc
>>>>>>> devel

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

<<<<<<< HEAD
    if (createInfo->fullscreen) {
        glfwGetMonitorWorkarea (
                windowMonitor,
                NULL, NULL,
                (int*) &window->windowSize.w,
                (int*) &window->windowSize.h);
    } else {
=======
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
>>>>>>> devel
        window->windowSize = createInfo->windowSize;
    }

    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
<<<<<<< HEAD
    // glfwWindowHint (GLFW_RESIZABLE, GLFW_FALSE); // RESIZABLE

    window->window = glfwCreateWindow(
=======

    window->window = glfwCreateWindow( // alloc
>>>>>>> devel
        window->windowSize.w,
        window->windowSize.h,
        window->title,
        windowMonitor,
        NULL);

<<<<<<< HEAD
    if (window->window == NULL) {
        glfwGetError (&glfwError);
=======
    if (window->window == NULL)
    {
        glfwGetError(&glfwError);
>>>>>>> devel
        log_fatal("Failed to create window. GLFW Error: %s", glfwError);
    }

    glfwSetWindowSizeCallback(window->window, &window_size_callback);

    window->resizing = false;

    return window;
}

<<<<<<< HEAD
SpiritResult spDestroyWindow (SpiritWindow window) {

    glfwDestroyWindow (window->window);
    // handle error
    if (glfwGetError (NULL) != GLFW_NO_ERROR) {
        const char *description;
        glfwGetError (&description);
        log_error("Failed to close window: %s", description);
        glfwTerminate ();
        return SPIRIT_FAILURE;
    }
    glfwTerminate();

    free(window);
=======
SpiritResult spDestroyWindow(SpiritWindow window)
{

    glfwDestroyWindow(window->window);
    free(window);
    glfwTerminate();
>>>>>>> devel

    log_verbose("Closing window");
    return SPIRIT_SUCCESS;
}

<<<<<<< HEAD
static bool g_glfwWindowWasResized = false;
static SpiritResolution g_windowSize = {
    .w = 0,
    .h = 0
};

void window_size_callback(GLFWwindow* window, int width, int height)
{
    g_glfwWindowWasResized = true;
    g_windowSize = (SpiritResolution) {
        .w = width,
        .h = height
    };
=======
static bool g_glfwWindowWasResized   = false;
static SpiritResolution g_windowSize = {.w = 0, .h = 0};

void window_size_callback(
    GLFWwindow *window __attribute_maybe_unused__, int width, int height)
{
    g_glfwWindowWasResized = true;
    g_windowSize           = (SpiritResolution){.w = width, .h = height};
>>>>>>> devel
}

SpiritWindowState spWindowGetState(SpiritWindow window)
{
<<<<<<< HEAD

    db_assert_msg(window && window->window, "Window cannot be null");
    const char *description = NULL;
=======
    const char *description = NULL;

>>>>>>> devel
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
<<<<<<< HEAD
    }

    if(!g_glfwWindowWasResized && window->resizing)
    {
        return SPIRIT_WINDOW_RESIZED;
    }

    if (g_glfwWindowWasResized)
    {
        g_glfwWindowWasResized = false;
        window->windowSize = g_windowSize;
        window->resizing = true;
        g_windowSize = (SpiritResolution) {};
=======
    }

    // the has stopped resizing
    if (!g_glfwWindowWasResized && window->resizing)
    {
        return SPIRIT_WINDOW_RESIZED;
    }

    // the window is being resized
    if (g_glfwWindowWasResized)
    {
        g_glfwWindowWasResized = false;
        window->windowSize     = g_windowSize;
        window->resizing       = true;
        g_windowSize           = (SpiritResolution){};
>>>>>>> devel
        return SPIRIT_WINDOW_RESIZING;
    }

    window->resizing = false;

    return SPIRIT_WINDOW_NORMAL;
}

SpiritResult spResizeWindow(SpiritWindow window, uint32_t w, uint32_t h)
{

<<<<<<< HEAD
    if (window->window == NULL) {
=======
    if (window->window == NULL)
    {
>>>>>>> devel
        log_error("spResizeWindow: window passed is NULL");
    }

    glfwSetWindowSize(window->window, w, h);

    return SPIRIT_SUCCESS;
}

<<<<<<< HEAD
SpiritResolution spWindowGetPixelSize (SpiritWindow window) {
=======
SpiritResolution spWindowGetPixelSize(SpiritWindow window)
{
>>>>>>> devel

    const char *glfwError;

    SpiritResolution res = {};
    int w, h;
<<<<<<< HEAD
    glfwGetFramebufferSize (window->window, &w, &h);
    if (glfwGetError (&glfwError) != GLFW_NO_ERROR) {
        log_error ("Failed to get framebuffer size. GLFW error: %s", glfwError);
=======
    glfwGetFramebufferSize(window->window, &w, &h);
    if (glfwGetError(&glfwError) != GLFW_NO_ERROR)
    {
        log_error("Failed to get framebuffer size. GLFW error: %s", glfwError);
>>>>>>> devel
        return res;
    }
    res.w = w, res.h = h;

    return res;
<<<<<<< HEAD
}

SpiritResolution spWindowGetSize(SpiritWindow window)
{
    return window->windowSize;
=======
>>>>>>> devel
}

SpiritResolution spWindowGetSize(SpiritWindow window)
{
    return window->windowSize;
}

VkSurfaceKHR spWindowGetSurface(SpiritWindow window, VkInstance instance)
{

    VkSurfaceKHR surface = NULL;

<<<<<<< HEAD
    if (glfwCreateWindowSurface (instance, window->window, NULL, &surface)) log_fatal("Failed to create window surface");
    db_assert_msg (surface, "Surface cannot be NULL");
    return surface;
}

SpiritWindowExtensions spWindowGetExtensions (SpiritWindow window) {
=======
    if (glfwCreateWindowSurface(instance, window->window, NULL, &surface))
        log_fatal("Failed to create window surface");
    db_assert_msg(surface, "Surface cannot be NULL");
    return surface;
}

SpiritWindowExtensions spWindowGetExtensions(SpiritWindow window)
{
>>>>>>> devel

    SpiritWindowExtensions out = {};

    out.names = glfwGetRequiredInstanceExtensions(&out.count);

    return out;
}
