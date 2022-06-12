#include <spirit_header.h>

// Create a spirit render context
// Automatically initialize a rendering system
// with all the components

// include components
#include "spirit_device.h"
#include "spirit_swapchain.h"
#include "spirit_renderpass.h"
#include "spirit_pipeline.h"
#include "spirit_window.h"

typedef struct t_SpiritContextCreateInfo
{

    // window
    SpiritResolution windowSize;
    const char *windowName;
    bool windowFullscreen;

    // device
    bool enableValidation; // should vulkan validation be initialized
    bool powerSaving; // should integrated GPU's be chosen

    // pipeline
    const char *vertexShader; // path to the vertex shader
    const char *fragmentShader; // path to the fragment shader



} SpiritContextCreateInfo;

SpiritContext spCreateContext(SpiritContextCreateInfo *createInfo);

SpiritResult spContextSubmitFrame(SpiritContext context);

SpiritResult spContextAddObject(void);

SpiritResult spContextAddMaterial(void);

SpiritResult spContextRemoveMaterial(void);

SpiritResult spDestroyContext(SpiritContext context);