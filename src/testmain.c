// test main function

#include <spirit_header.h>
#include "core/spirit_window.h"
#include "core/spirit_device.h"

#include "mem/spirit_memTree.h"

void mainlooptest (void);

int main (int argc, char **argv) {
    mainlooptest();

    return 0;
}
void mainlooptest (void) {
    SpiritWindowCreateInfo windowCreateInfo;
    windowCreateInfo.w = 800;
    windowCreateInfo.h = 600;
    windowCreateInfo.title = "Hello Spirit";
    windowCreateInfo.fullscreen = SPIRIT_FALSE;

    SpiritWindow window = spCreateWindow (windowCreateInfo);
    if (window == SPIRIT_NULL) {
        LOG_FATAL("No window");
    }

    SpiritDeviceCreateInfo deviceCreateInfo = {};
    deviceCreateInfo.enableValidation = SPIRIT_TRUE;
    deviceCreateInfo.verbose = SPIRIT_TRUE;
    deviceCreateInfo.powerSaveMode = SPIRIT_FALSE;

    deviceCreateInfo.appName = "TestApp";
    deviceCreateInfo.appVersion = VK_MAKE_VERSION(0, 0, 0);
    deviceCreateInfo.engineName = "Spirit Render";
    deviceCreateInfo.engineVersion = VK_MAKE_VERSION(0, 0, 0);

    deviceCreateInfo.windowExtensions = spWindowGetExtensions(window);

    SpiritDevice device = spCreateDevice (deviceCreateInfo);

    while (!spWindowShouldClose (window)) {
    }

    spDestroyWindow (window);
}
