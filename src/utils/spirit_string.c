#include "spirit_string.h"

// old
// // compare two strings
// SpiritBool spStringCmp(const char *str1, const char *str2, u32 max)
// {

//     u32 parsingIndex = 0; // current char
//     while (str1[parsingIndex] != '\0' && str2[parsingIndex] != '\0')
//     {
//         if (str1[parsingIndex] != str2[parsingIndex])
//             return SPIRIT_FALSE;
//         parsingIndex++;
//         if (max != 0 && max <= parsingIndex)
//             return SPIRIT_TRUE; // check if surpassing max characters to test
//     }

//     // strings are identical
//     if (str1[parsingIndex] == '\0' && str2[parsingIndex] == '\0')
//         return SPIRIT_TRUE;

//     return SPIRIT_FALSE;
// }

// old
// u32 spStringLen(const char *str, u32 max)
// {
//     u32 i = 0;
//     for (; str[i] != '\0'; i++)
//         if (max != 0 && i > max)
//             return max;
//     return i;
// }

// old
// SpiritResult spStringCpy(
//     const char *restrict src,
//     char *restrict dest,
//     u32 max)
// {
//     if (!src || !dest)
//         return SPIRIT_FAILURE;
// #define CHECK_UNDER_MAX (max == 0 || i < max)

//     u32 i;
//     for (i = 0; src[i] && CHECK_UNDER_MAX; i++)
//     {
//         dest[i] = src[i];
//         if (src[i] == '\0')
//             return SPIRIT_SUCCESS;
//     }

// #undef CHECK_UNDER_MAX

//     return SPIRIT_SUCCESS;
// }

// old
// SpiritBool spStringContains(
//     const char *container,
//     const char *token,
//     u32 max)
// {

//     char parsingChar = 0;
//     u32 i = 0; // current index of container

//     // u32 containerLength = spStringLen (container, 0);
//     u32 tokenLength = spStringLen(token, 0);

//     u32 matchedCount = 0; // number of correct chars in a row

//     while ((parsingChar = container[i]) != '\0')
//     {

//         if (parsingChar == token[matchedCount])
//         {
//             matchedCount++;
//             if (matchedCount >= tokenLength)
//                 return SPIRIT_TRUE;
//         }
//         else
//             matchedCount = 0;
//         i++;
//     }

//     return SPIRIT_FALSE;
// }

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
    SpiritBool inclusive)
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
    if (*length == 0) *length = lastSlicerInstance + 1 + inclusive;

    // null terminate string
    dest && (dest[lastSlicerInstance + inclusive] = '\0');

    return SPIRIT_SUCCESS;
}