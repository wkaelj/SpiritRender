#pragma once
#include <spirit_header.h>

// Create a window, with a given size
// 
// 
// Kael Johnston Feb 26 2022

//
// types
// 
// actual window information is kept private because of 
// platform specific details
typedef struct t_SpiritWindow *SpiritWindow;

typedef struct t_SpiritWindowCreateInfo {
    int32_t w, h; // size of window in screen units
    char   *title;
    bool    fullscreen;
} SpiritWindowCreateInfo;

typedef struct t_SpiritWindowExtensions {
    u32          count;
    const char **names;
} SpiritWindowExtensions;



// create a window
SpiritWindow spCreateWindow (SpiritWindowCreateInfo *createInfo);

// close and destroy window
SpiritResult spDestroyWindow (SpiritWindow window);

// check if a window should remain open or has been closed by the user
SpiritBool spWindowShouldClose (SpiritWindow window);

// resize window
SpiritResult spResizeWindow (SpiritWindow window, uint32_t w, uint32_t h);

// get window size in pixels not screen units
SpiritResult spWindowGetPixelSize (SpiritWindow window, uint32_t *w, uint32_t *h);

// get requried window extensions
SpiritWindowExtensions spWindowGetExtensions (SpiritWindow window);

// get window surface
VkSurfaceKHR spWindowGetSurface (SpiritWindow window, VkInstance instance);
