// test main function

#include <spirit_header.h>
#include "core/spirit_window.h"
#include "core/spirit_device.h"
#include "core/spirit_swapchain.h"
#include "core/spirit_pipeline.h"
#include "glsl-loader/glsl_loader.h"
#include "utils/spirit_file.h"
#include "utils/platform.h"

void mainlooptest (void);

int main (int argc, char **argv) {

    spPlatformSetExecutableFolder (argv[0]);

    SpiritShader o = compileShader (
        "#version 450\nlayout (location = 0) out vec4 outColor;layout(location = 0) in vec3 fragColor;\n \
void main () {outColor = vec4(fragColor, 1.0);}",
142, "frag", SPIRIT_SHADER_TYPE_FRAGMENT
    );

    db_assert(o.shader, "Shader failed");

    // mainlooptest ();

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

    SpiritPipeline pipeline = spCreatePipeline (device, &pipelineCreateInfo, NULL, NULL);

    while (!spWindowShouldClose (window));

    spDestroyPipeline (device, pipeline);
    spDestroySwapchain(swapchain, device);
    spDestroyDevice(device);
    spDestroyWindow (window);
}