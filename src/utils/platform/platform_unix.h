#pragma once
#include <spirit_header.h>

// platform file denominator ('/' or '\')
#define SPIRIT_PLATFORM_FOLDER_BREAK '/'

char *spPlatformGetCWD (void);

// set the folder that the program executable is stored in
// should be the first argument of the main function strings
void spPlatformSetExecutableFolder (char *name);

// get the folder the program executable is stored in
// includes trailing '/' character
// RETURNS the exectuable folder, but immutable
const char *spPlatformGetExecutableFolder (void);

// get the length of the executable directory string
// it is faster then calling strlen, because it is stored
u32 spPlatformGetExecutableFolderStrLen (void);

// convert a filename such as 'config.txt' to be relative to the
// exectuable rather then the CWD.
// 
// if max or ouput is 0/NULL it will just return the file length
// It also handles system conversions, so '/' are converted to '\' on win32
// RETURN - returns the length of the completed path
// NOTE - returns 0 on failure, and cannot succeed if max is insufficient
SpiritResult spPlatformLocalizeFileName (char *output, const char *path, u32* max);

// it is 64 bit because FUTURE PROOF YEAH BABYYYY!
time_t spPlatformGetTime (void);

// test if a file or folder exists, and can be read
bool spPlatformTestForFile (const char *filepath);

// return the size of a file, 0 for failure
u64 spPlatformTestFileSize (const char *filepath);

time_t spPlatformGetFileModifiedDate(const char *restrict filepath);

SpiritResult spPlatformCreateFolder(const char *path);