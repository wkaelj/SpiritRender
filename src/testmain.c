// test main function

#include <spirit_header.h>
#include "core/spirit_window.h"

#include "mem/spirit_memTree.h"

void mainlooptest (void);

int main (int argc, char **argv) {

    MemGap newGap = new_memgap(3, (void*) 0x1);
    MemGapTree gaps = &newGap;

    MemGap gap1 = {237, (void*) 0x02, NULL, NULL}, gap2 = {16, (void*) 0x03, NULL, NULL};
    memGapMap(gaps, gap1);
    memGapMap(gaps, gap2);

    LOG_DEBUG("Available ptr = %i", memGapGetPtr(gaps, 9).ptr);

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
