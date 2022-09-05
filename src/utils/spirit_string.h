#pragma once
#include <spirit_header.h>

// Functions to perform simple string operations
// I wrote my own library because I'm a real programmer.
// I don't use other people code, no matter how much better
// Unless im not smart enough to write it myself
//
// Kael Johnston Mar 7 2022
<<<<<<< HEAD
// 
// This didn't age well - Kael July 23 2022

// stolen from https://github.com/charlesnicholson/nanoprintf
// thanks (but not for the defines)
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

#undef NANOPRINTF_IMPLEMENTATION
#undef NANOPRINTF_USE_BINARY_FORMAT_SPECIFIERS
#undef NANOPRINTF_SNPRINTF_SAFE_TRIM_STRING_ON_OVERFLOW
#undef NANOPRINTF_USE_FIELD_WIDTH_FORMAT_SPECIFIERS
#undef NANOPRINTF_USE_PRECISION_FORMAT_SPECIFIERS
#undef NANOPRINTF_USE_LARGE_FORMAT_SPECIFIERS
#undef NANOPRINTF_USE_WRITEBACK_FORMAT_SPECIFIERS
#undef NANOPRINTF_USE_FLOAT_FORMAT_SPECIFIERS

/**
 * Remove the front of a string up to the last instance of a character.
 * Example(stripper='/'): "/home/user/document.txt" -> "document.txt"
 * 
 * @param dest may be a valid pointer to a string, or NULL. If dest is null, it 
 * will not be written to.
 * @param max may be a valid pointer to a unsigned int. If it's value is 0, it
 * will be written to with the length of the stripped string (including terminator).
 * If it's NULL, it will be ignored and there will be no maximum size for the output.
 * If it's not 0 or NULL, it will be used as the maximum length for the output string.
 * @param src must be a valid pointer to a string. It is the string that will be 
 * stripped.
 * @param stripper must be a valid character. It is the character that will be used
 * to strip the src string.
 * 
 * @author Kael Johnston
 */
SpiritResult spStringStrip (char *dest, u32 *max, const char *src, const char stripper);
=======
//
// This didn't age well - Kael July 23 2022

/**
 * Remove the front of a string up to the last instance of a character.
 * Example(stripper='/'): "/home/user/document.txt" -> "document.txt"
 *
 * @param dest may be a valid pointer to a string, or NULL. If dest is null, it
 * will not be written to.
 * @param max may be a valid pointer to a unsigned int. If it's value is 0, it
 * will be written to with the length of the stripped string (including
 * terminator). If it's NULL, it will be ignored and there will be no maximum
 * size for the output. If it's not 0 or NULL, it will be used as the maximum
 * length for the output string.
 * @param src must be a valid pointer to a string. It is the string that will be
 * stripped.
 * @param stripper must be a valid character. It is the character that will be
 * used to strip the src string.
 *
 * @author Kael Johnston
 */
SpiritResult spStringStrip(
    char *dest, u32 *max, const char *src, const char stripper);
>>>>>>> devel

/**
 * It will truncate a string at the last instance of a character. Examples:
 * Inclusive (slicer '/'): "/home/user/document.txt" -> "/home/user/"
 * Non-Inclusive (slicer '/'): "home/user/document.txt" -> "/home/user"
<<<<<<< HEAD
 * 
 * @param dest may be a valid pointer to a string, or NULL. If it is null, it will
 * not be written.
=======
 *
 * @param dest may be a valid pointer to a string, or NULL. If it is null, it
 * will not be written.
>>>>>>> devel
 * @param length may be a pointer to a valid unsigned int. If it's value is 0,
 * the length of the truncated string will be written to it. Otherwise, it will
 * be the maximum length of the output string (including terminator). If it is
 * NULL or 0 and dest is set, there will be no max length and the entire string
 * will be written.
 * @param slicer the character to cut the string off at.
<<<<<<< HEAD
 * @param inclusive whether or not the final string should include the last instance
 * of slicer.
 * 
 * @return SPIRIT_SUCCESS if the operation succeeded, SPIRIT_FAILURE otherwise
 * 
 * @author Kael Johnston
 */
SpiritResult spStringTruncate(
    char *restrict dest,
    u32 *restrict length,
    const char *restrict str, 
    const char slicer,
    bool inclusive);
=======
 * @param inclusive whether or not the final string should include the last
 * instance of slicer.
 *
 * @return SPIRIT_SUCCESS if the operation succeeded, SPIRIT_FAILURE otherwise
 *
 * @author Kael Johnston
 */
SpiritResult spStringTruncate(char *restrict dest, u32 *restrict length,
    const char *restrict str, const char slicer, bool inclusive);
>>>>>>> devel
