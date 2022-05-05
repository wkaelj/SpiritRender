#pragma once
#include <spirit_header.h>
// Functions to perform simple string operations
// I wrote my own library because I'm a real programmer.
// I don't use other people code, no matter how much better
// Unless im not smart enough to write it myself
//
// Kael Johnston Mar 7 2022

// stolen from https://github.com/charlesnicholson/nanoprintf
// thanks (but not for the goddamn defines)
#define NANOPRINTF_USE_FIELD_WIDTH_FORMAT_SPECIFIERS 0
#define NANOPRINTF_USE_PRECISION_FORMAT_SPECIFIERS 0
#define NANOPRINTF_USE_LARGE_FORMAT_SPECIFIERS 0
#define NANOPRINTF_USE_WRITEBACK_FORMAT_SPECIFIERS 0
#define NANOPRINTF_USE_FLOAT_FORMAT_SPECIFIERS 0

#define NANOPRINTF_IMPLEMENTATION
#define NANOPRINTF_USE_BINARY_FORMAT_SPECIFIERS 1
#define NANOPRINTF_SNPRINTF_SAFE_TRIM_STRING_ON_OVERFLOW 1
#define NANOPRINTF_USE_FIELD_WIDTH_FORMAT_SPECIFIERS 0

#include "nanoprintf.h"

// #undef NANOPRINTF_IMPLEMENTATION
// #undef NANOPRINTF_USE_BINARY_FORMAT_SPECIFIERS
// #undef NANOPRINTF_SNPRINTF_SAFE_TRIM_STRING_ON_OVERFLOW
// #undef NANOPRINTF_USE_FIELD_WIDTH_FORMAT_SPECIFIERS
// #undef NANOPRINTF_USE_PRECISION_FORMAT_SPECIFIERS
// #undef NANOPRINTF_USE_LARGE_FORMAT_SPECIFIERS
// #undef NANOPRINTF_USE_WRITEBACK_FORMAT_SPECIFIERS
// #undef NANOPRINTF_USE_FLOAT_FORMAT_SPECIFIERS

// compare two strings. Will return true if the same, otherwise false.
// It will only parse up to maxChars, if maxChars is nonzero
SpiritBool spStringCmp(const char *str1, const char *str2, u32 maxChars);

// return the length of str, capped by max
u32 spStringLen (const char *str, u32 max);

// copy the contents of src into dest
SpiritResult spStringCpy (
    const char *restrict src, 
    char *restrict       dest, 
    u32                  max);

// check if container contains token
SpiritBool spStringContains(
    const char *container,
    const char *token,
    u32 max);

// it will strip everything before the last instance of stripper
// Example: "/home/path/to/file/config.txt" -> "config.txt"
const char *spStringStrip (const char *filename, const char stripper);

// cut off everything after the last instance of slicer
// Example: "/home/path/to/file/file.txt -> /home/path/to/file/"
SpiritResult spStringTruncate(char *str, const char slicer);