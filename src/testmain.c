// test main function

#include <spirit_header.h>
#include "core/spirit_window.h"

void mainlooptest (void);

int main (int argc, char **argv) {
    mainlooptest ();
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

    while (!spWindowShouldClose (window)) {
    }

    spDestroyWindow (window);
}