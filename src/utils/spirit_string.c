#include "spirit_string.h" 


// compare two strings
SpiritBool spStringCmp (const char *str1, const char *str2, u32 max) {

    u32 parsingIndex = 0; // current char
    while (str1[parsingIndex] != '\0' && str2[parsingIndex] != '\0') {
        if (str1[parsingIndex] != str2[parsingIndex]) return SPIRIT_FALSE;
        parsingIndex++;
        if (max != 0 && max <= parsingIndex) return SPIRIT_TRUE; // check if surpassing max characters to test
    }

    // strings are identical
    if (str1[parsingIndex] == '\0' && str2[parsingIndex] == '\0') return SPIRIT_TRUE;

    return SPIRIT_FALSE;
}

u32 spStringLen (const char *str, u32 max) {
    u32 i = 0;
    for (; str[i] != '\0'; i++) if (max != 0 && i > max) return max;
    return i;
}

SpiritBool spStringContains (
    const char *container,
    const char *token,
    u32 max) {
    
    char parsingChar = 0;
    u32  i = 0; // current index of container

    // u32 containerLength = spStringLen (container, 0);
    u32 tokenLength =     spStringLen (token,     0);

    u32 matchedCount = 0; // number of correct chars in a row

    while ((parsingChar = container[i]) != '\0') {

        if (parsingChar == token[matchedCount]) {
            matchedCount++;
            if (matchedCount >= tokenLength) return SPIRIT_TRUE;
        } else matchedCount = 0;
        i++;
    }

    return SPIRIT_FALSE;

        
}

// it will strip everything before the last instance of stripper
// Example: "/home/path/to/file/config.txt" -> "config.txt"
char *spStringStrip (const char *filename, const char stripper)
{

    char *out = filename;
    // iterate through filename
    for (size_t i = 0;true; i++)
    {
        if (out[i] == '\0')
            return filename;
        else if (out[i] == stripper)
            return spStringStrip ((char *) &out[++i], stripper);
    }
    return out;
}