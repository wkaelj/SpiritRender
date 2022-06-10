// test main function

#include <spirit_header.h>

// types
#include "core/spirit_window.h"
#include "core/spirit_device.h"
#include "core/spirit_swapchain.h"
#include "core/spirit_pipeline.h"
#include "core/spirit_renderpass.h"

// utils
#include "glsl-loader/glsl_loader.h"
#include "utils/spirit_file.h"
#include "utils/platform.h"

void mainlooptest (void);

int main (int argc, char **argv) {

    spPlatformSetExecutableFolder (argv[0]);

    const char *path = "shaders/simple_shader.frag";
    u64 srcLen = spReadFileSize(path);
    log_debug("Src length = %u", srcLen);
    char src[srcLen + 1];
    spReadFileText(src, path, &srcLen);
    src[srcLen] = '\0';
    db_assert(srcLen == strlen(src), "");

    SpiritShader o = spCompileShader (src, srcLen, "frag", SPIRIT_SHADER_TYPE_FRAGMENT);

    db_assert(o.shader, "Shader failed");

    spDestroyShader(o);

    mainlooptest ();

    return 0;
}

void mainlooptest (void) {

    // create window
    SpiritWindowCreateInfo windowCreateInfo;
    windowCreateInfo.w = 800;
    windowCreateInfo.h = 600;
    windowCreateInfo.title = "Hello Spirit";
    windowCreateInfo.fullscreen = SPIRIT_FALSE;

    SpiritWindow window = spCreateWindow (&windowCreateInfo);
    db_assert(window, "Must have window");

    // create device
    SpiritDeviceCreateInfo deviceCreateInfo = {};
    deviceCreateInfo.enableValidation = SPIRIT_TRUE;
    deviceCreateInfo.powerSaveMode = SPIRIT_FALSE;

    deviceCreateInfo.appName = "TestApp";
    deviceCreateInfo.appVersion = VK_MAKE_VERSION(0, 0, 0);
    deviceCreateInfo.engineName = "Spirit Render";
    deviceCreateInfo.engineVersion = VK_MAKE_VERSION(0, 0, 0);

    deviceCreateInfo.windowExtensions = spWindowGetExtensions(window);
    deviceCreateInfo.window = window;

    const char *deviceExtensions[1] = {
        VK_KHR_SWAPCHAIN_EXTENSION_NAME,
    };

    deviceCreateInfo.requiredDeviceExtensions = deviceExtensions;
    deviceCreateInfo.requiredDeviceExtensionCount = 1;
    SpiritDevice device = spCreateDevice (&deviceCreateInfo);
    db_assert(device, "Must have device");

    // create swapchain
    SpiritSwapchainCreateInfo swapCreateInfo = {};
    spWindowGetPixelSize(window, &swapCreateInfo.windowWidthPx, &swapCreateInfo.windowHeightPx);

    SpiritSwapchain swapchain = spCreateSwapchain(swapCreateInfo, device, NULL);
    db_assert(swapchain, "Must have swapchain");

    SpiritRenderPassCreateInfo renderPassCreateInfo = {};

    SpiritRenderPass renderPass = spCreateRenderPass(&renderPassCreateInfo, device, swapchain);

    SpiritPipelineCreateInfo pipelineCreateInfo = {};
    pipelineCreateInfo.shaderFilePathCount = 2;
    SpiritShader shaders[2];
    shaders[0].path = "shaders/simple_shader.frag";
    shaders[0].type = SPIRIT_SHADER_TYPE_FRAGMENT;
    shaders[1].path = "shaders/simple_shader.vert";
    shaders[1].type = SPIRIT_SHADER_TYPE_VERTEX;
    pipelineCreateInfo.shaderFilePaths = shaders;
    pipelineCreateInfo.windowWidth = swapCreateInfo.windowWidthPx;
    pipelineCreateInfo.windowHeight = swapCreateInfo.windowHeightPx;
    pipelineCreateInfo.renderWidth = swapCreateInfo.windowWidthPx;
    pipelineCreateInfo.renderHeight = swapCreateInfo.windowHeightPx;

    SpiritPipeline pipeline = spCreatePipeline (
        device, 
        &pipelineCreateInfo,
        swapchain,
        renderPass, 
        NULL);

    while (!spWindowShouldClose (window));

    spDestroyPipeline (device, pipeline);
    spDestroyRenderPass(renderPass, device);
    spDestroySwapchain(swapchain, device);
    spDestroyDevice(device);
    spDestroyWindow (window);
}