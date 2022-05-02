#pragma once
#include <spirit_header.h>

// Utilities usefull in file I/O
//
// 
// Kael Johnston May 1 2022

// read a binary file
// if dest it null it will only do size
// if size is non 0 it will read a maximum of size bytes
// if dest is null it does not matter if size is non 0
SpiritResult spReadFileBinary (
    void *restrict dest,
    const char    *filepath,
    u64 *restrict  size);

// read a text file into a string
// if dest is null it will only read size
// if size is non 0 it will read a maximum of size
// if dest is null it does not matter if size is non 0
SpiritResult spReadFileText (
    char         *dest,
    const char   *filepath,
    u64 *restrict length);