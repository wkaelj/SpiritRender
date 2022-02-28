#include "spirit_window.h"
#include <GLFW/glfw3.h>

#define GLFW_INCLUDE_VULKAN
struct t_SpiritWindow {
    GLFWwindow *window;
    int32_t w, h;
    char *title;
};

SpiritWindow spCreateWindow (SpiritWindowCreateInfo createInfo) {

    SpiritWindow window = new_var(SpiritWindow); 

    const char *glfwError;
    if (glfwInit () != GLFW_TRUE) {
        glfwGetError (&glfwError);
        LOG_ERROR("GLFW error: %s", glfwError);
        return SPIRIT_NULL;
    }

    window->title = createInfo.title;

    GLFWmonitor *windowMonitor = NULL;
    if (createInfo.fullscreen) windowMonitor = glfwGetPrimaryMonitor ();
    if (createInfo.fullscreen && windowMonitor == NULL) {
        glfwGetError (&glfwError);
        LOG_ERROR("No fullscreen monitor available! GLFW error %s", glfwError);
        createInfo.fullscreen = SPIRIT_FALSE;
    }

    if (createInfo.fullscreen) {
        glfwGetMonitorWorkarea (windowMonitor, NULL, NULL,  &window->w, &window->h);
    } else {
        window->w = createInfo.w;
        window->h = createInfo.h;
    }
    glfwWindowHint (GLFW_CLIENT_API, GLFW_NO_API);
    glfwWindowHint (GLFW_RESIZABLE, GLFW_FALSE);

    window->window = glfwCreateWindow (window->w, window->h, window->title, windowMonitor, NULL);
    if (window->window == NULL) {
        glfwGetError (&glfwError);
        LOG_FATAL("Failed to create window. GLFW Error: %s", glfwError);
    } else LOG_DEBUG("Created Window");

    return window;
}

SpiritResult spDestroyWindow (SpiritWindow window) {

    glfwDestroyWindow (window->window);
    if (glfwGetError (NULL) != GLFW_NO_ERROR) {
        const char *description;
        glfwGetError (&description);
        LOG_ERROR(description);
    }
    glfwTerminate ();

    

    return SPIRIT_SUCCESS;
}

SpiritBool spWindowShouldClose (SpiritWindow window) {
    const char *description;
    assert (window->window != NULL);
    glfwPollEvents ();
    if (glfwGetError (&description) != GLFW_NO_ERROR) {
        LOG_ERROR("GLFW error: %s", description);
        return SPIRIT_FAILURE;
    }
    if (glfwWindowShouldClose (window->window)) {
        if (glfwGetError (&description) != GLFW_NO_ERROR) {
            LOG_ERROR("GLFW error: %s", description);
            return SPIRIT_FAILURE;
        }
        LOG_DEBUG("Closing window");
        return SPIRIT_TRUE;
    } else {
        return SPIRIT_FALSE;
    }
}

SpiritResult spResizeWindow (SpiritWindow window, uint32_t w, uint32_t h) {

    if (window->window == SPIRIT_NULL) {
        LOG_ERROR("spResizeWindow: window passed is NULL");
    }

    glfwSetWindowSize (window->window, w, h);

    return SPIRIT_SUCCESS;
}

SpiritResult spWindowGetPixelSize (SpiritWindow window, uint32_t *w, uint32_t *h) {

    const char *glfwError;
    glfwGetFramebufferSize (window->window, (int32_t*) w, (int32_t*) h);
    if (glfwGetError (&glfwError) != GLFW_NO_ERROR) {
        LOG_ERROR ("Failed to get framebuffer size. GLFW error: %s", glfwError);
        return SPIRIT_FAILURE;
    }
    return SPIRIT_SUCCESS;
}