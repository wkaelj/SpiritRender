#include "spirit_file.h"
#include <stdio.h>

SpiritResult spReadFileBinary (
    void *restrict dest,
    const char    *filepath,
    u64 *restrict  size)
{

    db_assert (size, "Size cannot have a NULL value"); // size cannot be null

    // open file
    FILE *file = fopen (filepath, "r");
    if (!file)
    {
        log_error("Failed to open file '%s'.", filepath);
        return SPIRIT_NULL;
    }

    // get file size
    fseek (file, 0, SEEK_END);
    const u64 fileSize = ftell (file);
    fseek (file, 0, SEEK_SET);

    // set the filesize if it has not already been set
    if (*size == 0) *size = fileSize;

    // read/store contents
    if (dest)
    {
        fread(dest, *size, 1, file);
    }

    fclose (file);
    
    return SPIRIT_SUCCESS;
}

SpiritResult spReadFileText (
    char         *dest,
    const char   *filepath,
    u64 *restrict length)
{

    db_assert (length, "Length must have be a valid pointer");

    FILE *file = fopen (filepath, "r");
    if (!file)
    {
        log_error("Failed to open file '%s'.", filepath);
        return SPIRIT_NULL;
    }

    // get file size
    fseek (file, 0, SEEK_END);
    const u64 fileSize = ftell (file);
    fseek (file, 0, SEEK_SET);

    if (*length == 0) *length = fileSize + 1;

    // read file
    if (dest)
    {
        fread (dest, *length - 1, 1, file);
        dest[*length] = '\0';
    }

    fclose (file);
    return SPIRIT_SUCCESS;
}