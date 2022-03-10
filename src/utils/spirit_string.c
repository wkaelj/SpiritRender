#include "spirit_string.h" 


// compare two strings
SpiritBool spStringCmp (const char *str1, const char *str2, u32 maxChars) {

    u32 parsingIndex = 0; // current char
    while (str1[parsingIndex] != '\0' && str2[parsingIndex] != '\0') {
        if (str1[parsingIndex] != str2[parsingIndex]) return SPIRIT_FALSE;
        parsingIndex++;
        if (maxChars != 0 && maxChars <= parsingIndex) return SPIRIT_TRUE; // check if surpassing max characters to test
    }

    // strings are identical
    if (str1[parsingIndex] == '\0' && str2[parsingIndex] == '\0') return SPIRIT_TRUE;

    return SPIRIT_FALSE;
}
