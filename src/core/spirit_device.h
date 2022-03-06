#pragma once
#include <spirit_header.h>

#include "spirit_window.h"

// Create and return rendering device
//
//
// Kael Johnston Feb 26 2022

// create a spirit device
SpiritDevice spCreateDevice (SpiritDeviceCreateInfo createInfo);

// destroy spirit device
SpiritResult spDestroyDevice (SpiritDevice device);

