#include "spirit_string.h"

// it will strip everything before the last instance of stripper
// Example: "/home/path/to/file/config.txt" -> "config.txt"
const char *spStringStrip(const char *filename, const char stripper)
{

    // iterate through filename
    for (size_t i = 0; true; i++)
    {
        if (filename[i] == '\0')
            return filename;
        else if (filename[i] == stripper)
            return spStringStrip((char *)&filename[++i], stripper);
    }
    return filename;
}

// TODO rewrite using std lib
// cut off everything after the last instance of slicer
// Example: "/home/path/to/file/file.txt -> /home/path/to/file/"
SpiritResult spStringTruncate(
    char *restrict dest,
    u32 *restrict length,
    const char *restrict str,
    const char slicer,
    bool inclusive)
{

    db_assert(length, "Length value cannot be null");
    db_assert(inclusive == 0 ^ inclusive == 1, "'inclusive' must be 1 or 0");

    if (!length || !inclusive) return SPIRIT_FAILURE;

    u32 lastSlicerInstance = 0;

    // find the length of the string, and clamp it
    u32 strLength = strlen (str);
    if  (*length && strLength > *length) strLength = *length;

    // find the last instance of a symbol
    for (u32 i = 0; str[i] != '\0' && i < strLength; i++)
    {
        if (str[i] == slicer) lastSlicerInstance = i;
    }

    // reset length if there is no instances of slicer
    if (lastSlicerInstance == 0) lastSlicerInstance = strLength;

    // if there is a destination, copy the final string into it
    if (dest)
    {
        strncpy (dest, str, lastSlicerInstance + inclusive);
    }

    // reset length if it is 0
    if (*length == 0) *length = lastSlicerInstance + inclusive;

    // null terminate string
    dest && (dest[lastSlicerInstance + inclusive] = '\0');

    return SPIRIT_SUCCESS;
}