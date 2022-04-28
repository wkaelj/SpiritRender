#include "spirit_window.h"
#include <GLFW/glfw3.h>

#define GLFW_INCLUDE_VULKAN
struct t_SpiritWindow {
    GLFWwindow *window;
    int32_t w, h;
    char *title;
};

SpiritWindow spCreateWindow (SpiritWindowCreateInfo *createInfo) {

    SpiritWindow window = new_var(struct t_SpiritWindow); 

    const char *glfwError;
    if (glfwInit () != GLFW_TRUE) {
        glfwGetError (&glfwError);
        log_error("GLFW error: %s", glfwError);
        return SPIRIT_NULL;
    }

    window->title = createInfo->title;

    GLFWmonitor *windowMonitor = NULL;
    if (createInfo->fullscreen) windowMonitor = glfwGetPrimaryMonitor ();
    if (createInfo->fullscreen && windowMonitor == NULL) {
        glfwGetError (&glfwError);
        log_error("No fullscreen monitor available! GLFW error %s", glfwError);
        createInfo->fullscreen = SPIRIT_FALSE;
    }

    if (createInfo->fullscreen) {
        glfwGetMonitorWorkarea (windowMonitor, NULL, NULL,  &window->w, &window->h);
    } else {
        window->w = createInfo->w;
        window->h = createInfo->h;
    }

    glfwWindowHint (GLFW_CLIENT_API, GLFW_NO_API);
    glfwWindowHint (GLFW_RESIZABLE, GLFW_FALSE); // RESIZABLE

    window->window = glfwCreateWindow (
        window->w, 
        window->h, 
        window->title, 
        windowMonitor, 
        NULL);
    
    if (window->window == NULL) {
        glfwGetError (&glfwError);
        log_fatal("Failed to create window. GLFW Error: %s", glfwError);
    } else log_debug("Created Window");

    return window;
}

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
    glfwTerminate ();

    dalloc(window);

    log_verbose("Closing window");
    return SPIRIT_SUCCESS;
}

SpiritBool spWindowShouldClose (SpiritWindow window) {
    const char *description;
    assert (window->window != NULL);
    glfwPollEvents ();
    if (glfwGetError (&description) != GLFW_NO_ERROR) {
        log_error("GLFW error: %s", description);
        return SPIRIT_FAILURE;
    }
    if (glfwWindowShouldClose (window->window)) {
        if (glfwGetError (&description) != GLFW_NO_ERROR) {
            log_error("GLFW error: %s", description);
            return SPIRIT_FAILURE;
        }
        return SPIRIT_TRUE;
    } else {
        return SPIRIT_FALSE;
    }
}

SpiritResult spResizeWindow (SpiritWindow window, uint32_t w, uint32_t h) {

    if (window->window == SPIRIT_NULL) {
        log_error("spResizeWindow: window passed is NULL");
    }

    glfwSetWindowSize (window->window, w, h);

    return SPIRIT_SUCCESS;
}

SpiritResult spWindowGetPixelSize (SpiritWindow window, uint32_t *w, uint32_t *h) {

    const char *glfwError;
    glfwGetFramebufferSize (window->window, w, h);
    if (glfwGetError (&glfwError) != GLFW_NO_ERROR) {
        log_error ("Failed to get framebuffer size. GLFW error: %s", glfwError);
        return SPIRIT_FAILURE;
    }
    return SPIRIT_SUCCESS;
}

VkSurfaceKHR spWindowGetSurface (SpiritWindow window, VkInstance instance) {

    VkSurfaceKHR surface = SPIRIT_NULL;

    if (glfwCreateWindowSurface (instance, window->window, SPIRIT_NULL, &surface)) log_fatal("Failed to create window surface");
    db_assert (surface, "Surface cannot be null");
    return surface;
}

SpiritWindowExtensions spWindowGetExtensions (SpiritWindow window) {
    
    SpiritWindowExtensions out = {};

    out.names = glfwGetRequiredInstanceExtensions(&out.count);

    return out;
}
