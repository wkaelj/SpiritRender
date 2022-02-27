#pragma once
#include <spirit_header.h>

// Create a window, with a given size
// 
// 
// Kael Johnston Feb 26 2022

// create a window
SpiritWindow spCreateWindow (SpiritWindowCreateInfo createInfo);

// close and destroy window
SpiritReturns spDestroyWindow (SpiritWindow window);

// check if a window should remain open or has been closed by the user
SpiritReturns spWindowShouldClose (SpiritWindow window);

