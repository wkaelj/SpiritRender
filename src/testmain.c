// test main function

#include <spirit_header.h>
#include "core/spirit_window.h"

int main (int argc, char **argv) {

    SpiritWindowCreateInfo windowCreateInfo;
    windowCreateInfo.w = 800;
    windowCreateInfo.h = 600;
    windowCreateInfo.title = "Hello Spirit";
    windowCreateInfo.fullscreen = SPIRIT_TRUE;

    SpiritWindow window = spCreateWindow (windowCreateInfo);
    if (window == SPIRIT_NULL) {
        LOG_FATAL("No window");
        return 1;
    }

    while (!spWindowShouldClose (window)) {
    }

    spDestroyWindow (window);
    
    return 0;
}