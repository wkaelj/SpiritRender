#include "spirit_string.h"

// compare two strings
SpiritBool spStringCmp(const char *str1, const char *str2, u32 max)
{

    u32 parsingIndex = 0; // current char
    while (str1[parsingIndex] != '\0' && str2[parsingIndex] != '\0')
    {
        if (str1[parsingIndex] != str2[parsingIndex])
            return SPIRIT_FALSE;
        parsingIndex++;
        if (max != 0 && max <= parsingIndex)
            return SPIRIT_TRUE; // check if surpassing max characters to test
    }

    // strings are identical
    if (str1[parsingIndex] == '\0' && str2[parsingIndex] == '\0')
        return SPIRIT_TRUE;

    return SPIRIT_FALSE;
}

u32 spStringLen(const char *str, u32 max)
{
    u32 i = 0;
    for (; str[i] != '\0'; i++)
        if (max != 0 && i > max)
            return max;
    return i;
}

SpiritResult spStringCpy(
    const char *restrict src,
    char *restrict dest,
    u32 max)
{
    if (!src || !dest)
        return SPIRIT_FAILURE;
#define CHECK_UNDER_MAX (max == 0 || i < max)

    u32 i;
    for (i = 0; src[i] && CHECK_UNDER_MAX; i++)
    {
        dest[i] = src[i];
        if (src[i] == '\0')
            return SPIRIT_SUCCESS;
    }

#undef CHECK_UNDER_MAX

    return SPIRIT_SUCCESS;
}

SpiritBool spStringContains(
    const char *container,
    const char *token,
    u32 max)
{

    char parsingChar = 0;
    u32 i = 0; // current index of container

    // u32 containerLength = spStringLen (container, 0);
    u32 tokenLength = spStringLen(token, 0);

    u32 matchedCount = 0; // number of correct chars in a row

    while ((parsingChar = container[i]) != '\0')
    {

        if (parsingChar == token[matchedCount])
        {
            matchedCount++;
            if (matchedCount >= tokenLength)
                return SPIRIT_TRUE;
        }
        else
            matchedCount = 0;
        i++;
    }

    return SPIRIT_FALSE;
}

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

// cut off everything after the last instance of slicer
// Example: "/home/path/to/file/file.txt -> /home/path/to/file/"
SpiritResult spStringTruncate(char *str, const char slicer)
{

    u32 lastInstanceIndex = 0; // last instance of slicer

    for (u32 i = 0; str[i] != '\0'; i++)
    {
        if (str[i] == slicer)
        {
            lastInstanceIndex = i;
        }
    }

    // this is safe, because if i = '\0' the loop ends
    // only goes if lastInstanceIndex isn't 0
    if (lastInstanceIndex)
        str[lastInstanceIndex + 1] = '\0';

    return SPIRIT_SUCCESS;
}