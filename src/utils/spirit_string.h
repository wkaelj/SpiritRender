#pragma once
#include <spirit_header.h>

// Functions to perform simple string operations
// I wrote my own library because I'm a real programmer.
// I don't use other people code, no matter how much better
// Unless im not smart enough to write it myself
//
// Kael Johnston Mar 7 2022

// compare two strings. Will return true if the same, otherwise false.
// It will only parse up to maxChars, if maxChars is nonzero
SpiritBool spStringCmp (const char *str1, const char *str2, u32 maxChars);

// return the length of str, capped by max
u32 spStringLen(const char *str, u32 max);

// check if container contains token
SpiritBool spStringContains(
    const char *container,
    const char *token,
    u32 max);

// it will strip everything before the last instance of stripper
// Example: "/home/path/to/file/config.txt" -> "config.txt"
const char *spStringStrip(const char *filename, const char stripper);