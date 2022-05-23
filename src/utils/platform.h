#pragma once
#include <spirit_header.h>

// Platform specific systems
// 
// 
// Kael Johnston May 1 2022

#ifdef _WIN32
#include "platform/platform_win32.h"
#elif __unix
#include "platform/platform_unix.h"
#elif __APPLE__
#include "platform/platform_unix.h"
#else
#error Unsupported platform
#endif