#pragma once
#include <spirit_header.h>

// Utilities usefull in file I/O
//
//
// Kael Johnston May 1 2022

// ===================================
//              Reading
// ===================================

// return the size of a file
// using the stat function
// returns 0 if the file does not exist
// returns 1 on a failure
u64 spReadFileSize(const char *filepath);

// test if a file exits
// returns size of file, 0 if it does not exist
bool spReadFileExists(const char *filepath);

// read a binary file
// if dest it NULL it will only do size
// if size is non 0 it will read a maximum of size bytes
// if dest is NULL it does not matter if size is non 0
SpiritResult spReadFileBinary(
    void          *dest,
    const char    *filepath,
    u64 * size);

// read a text file into a string
// if dest is NULL it will only read size
// if size is non 0 it will read a maximum of size
// if dest is NULL it does not matter if size is non 0
SpiritResult spReadFileText(
    char *dest,
    const char *filepath,
    u64 *length);

// read the last time a file was modified
// unix time
time_t spReadFileModifiedTime(const char *filepath);

// ===================================
//              Writing
// ===================================

// Write data to a file
// will create the file if it does not exist,
// and will overwrite the file if it already exists
// 
// path - Path the the new file
// contents - the data to write to the file
// size - the size of contents in bytes
SpiritResult spWriteFileBinary(
    const char *path, 
    const void *contents,
    const u32   size);

// creates a file and all parent files, if they do not exist
// it will treat directories not starting with '.' or '/' relative
// to the exectuable location
// it will always be a FOLDER! adding .txt will just make folder.txt
// it will not make a text doc
SpiritResult spWriteFileFolder (const char *path);
