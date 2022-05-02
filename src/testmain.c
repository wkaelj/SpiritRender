// test main function

#include <spirit_header.h>
#include "core/spirit_window.h"
#include "core/spirit_device.h"
#include "core/spirit_swapchain.h"
#include "glsl-loader/glsl_loader.h"
#include "utils/spirit_file.h"
#include "utils/platform.h"

void mainlooptest (void);

int main (int argc, char **argv) {

    spSetExecutableFolder (argv[0]);
    
    // mainlooptest ();

    char *container = "Hello /World.vert";

    if (spStringContains (container, "World", 18)) {
        log_info ("Container works");
    }


    log_info (spStringStrip (container, '/'));
    log_info (spStringStrip(container, '.'));

    SpiritShader vert, frag;
    frag = loadSourceShader (
        "shaders/simple_shader.frag", 
        SPIRIT_SHADER_TYPE_FRAGMENT);
    vert = loadSourceShader (
        "shaders/simple_shader.vert",
        SPIRIT_SHADER_TYPE_VERTEX);


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

    SpiritSwapchain swapchain = spCreateSwapchain(swapCreateInfo, device, SPIRIT_NULL);
    db_assert(swapchain, "Must have swapchain");

    while (!spWindowShouldClose (window));

    spDestroySwapchain(swapchain, device);
    spDestroyDevice(device);
    spDestroyWindow (window);
}