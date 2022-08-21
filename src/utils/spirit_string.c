#include "spirit_string.h"

// it will strip everything before the last instance of stripper
// Example: "/home/path/to/file/config.txt" -> "config.txt"
SpiritResult spStringStrip (char *dest, u32 *max, const char *src, const char stripper)
{
    db_assert_msg(src, "src must be a valid pointer to a string!");
    if (!src) return SPIRIT_FAILURE;

    u32 lastInstanceOfSlicer = 0;
    u32 srcLen = 0;
    for (u32 i = 0; src[i] != '\0'; i++)
    {
        if (src[i] == stripper) lastInstanceOfSlicer = i;
        srcLen = i; // likely inefficient
    }

    if (max && *max == 0) *max = srcLen - lastInstanceOfSlicer + 1;
    // X|X|L|O|O|\0

    if (dest) strncpy(
        dest,
        &src[lastInstanceOfSlicer + 1],
        max ? *max : srcLen - lastInstanceOfSlicer + 1); // fallback length

    return SPIRIT_SUCCESS;
}

// cut off everything after the last instance of slicer
// Example: "/home/path/to/file/file.txt -> /home/path/to/file/"
SpiritResult spStringTruncate(
    char *dest,
    u32 *max,
    const char *str,
    const char slicer,
    bool inclusive)
{

    db_assert_msg(str, "String value cannot be NULL");

    if (!str) return SPIRIT_FAILURE;

    u32 lastSlicerInstance = 0;

    // find the length of the string, and clamp it
    u32 strLength = strlen(str);

    if  (max && *max && strLength > *max) strLength = *max;

    // find the last instance of a symbol
    for (u32 i = 0; str[i] != '\0' && i < strLength; i++)
    {
        if (str[i] == slicer) lastSlicerInstance = i;
    }

    // reset max if there is no instances of slicer
    if (lastSlicerInstance == 0) lastSlicerInstance = strLength;

    // reset max if it is 0
    if (*max == 0) *max = lastSlicerInstance + inclusive + 1;

    // if there is a destination, copy the final string into it
    if (dest)
    {
        // pick shorter, max or slicer
        u32 l = (max && *max < lastSlicerInstance + inclusive) ?
            *max : lastSlicerInstance + inclusive;
        strncpy(dest, str, l);
    }

    // NULL terminate string
    dest && (dest[lastSlicerInstance + inclusive] = '\0');

    return SPIRIT_SUCCESS;
}