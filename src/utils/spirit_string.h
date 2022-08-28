#pragma once
#include <spirit_header.h>
// Functions to perform simple string operations
// I wrote my own library because I'm a real programmer.
// I don't use other people code, no matter how much better
// Unless im not smart enough to write it myself
//
// Kael Johnston Mar 7 2022
//
// This didn't age well - Kael July 23 2022

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

/**
 * It will truncate a string at the last instance of a character. Examples:
 * Inclusive (slicer '/'): "/home/user/document.txt" -> "/home/user/"
 * Non-Inclusive (slicer '/'): "home/user/document.txt" -> "/home/user"
 *
 * @param dest may be a valid pointer to a string, or NULL. If it is null, it will
 * not be written.
 * @param length may be a pointer to a valid unsigned int. If it's value is 0,
 * the length of the truncated string will be written to it. Otherwise, it will
 * be the maximum length of the output string (including terminator). If it is
 * NULL or 0 and dest is set, there will be no max length and the entire string
 * will be written.
 * @param slicer the character to cut the string off at.
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
