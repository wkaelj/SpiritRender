#include "spirit_file.h"
#include <stdio.h>
#include <sys/stat.h>

// just a wrapper, to help keep the file utilities organized
u64 spReadFileSize(const char *filepath)
{
    return spPlatformTestFileSize(filepath);
}

SpiritBool spReadFileExists(const char *filepath)
{
    return spPlatformTestFileSize(filepath);
}

SpiritResult spReadFileBinary(
    void *restrict dest,
    const char *filepath,
    u64 *restrict size)
{

    db_assert(size, "Size cannot have a NULL value"); // size cannot be null

    // open file
    FILE *file = fopen(filepath, "r");
    if (!file)
    {
        log_error("Failed to open file '%s'.", filepath);
        return SPIRIT_FAILURE;
    }

    // get file size
    fseek(file, 0, SEEK_END);
    const u64 fileSize = ftell(file);
    fseek(file, 0, SEEK_SET);

    // set the filesize if it has not already been set
    if (*size == 0)
        *size = fileSize;

    // read/store contents
    if (dest)
    {
        fread(dest, *size, 1, file);
    }

    fclose(file);

    return SPIRIT_SUCCESS;
}

SpiritResult spReadFileText(
    char *dest,
    const char *filepath,
    u64 *restrict length)
{

    db_assert(length, "Length must have be a valid pointer");

    FILE *file = fopen(filepath, "r");
    if (!file)
    {
        log_error("Failed to open file '%s'.", filepath);
        return SPIRIT_FAILURE;
    }

    // get file size
    fseek(file, 0, SEEK_END);
    const u64 fileSize = ftell(file);
    fseek(file, 0, SEEK_SET);

    if (*length == 0)
        *length = fileSize + 1;

    // read file
    if (dest)
    {
        fread(dest, *length - 1, 1, file);
        dest[*length] = '\0';
    }

    fclose(file);
    return SPIRIT_SUCCESS;
}

SpiritResult spWriteFileBinary(
    const char *path,
    const void *contents,
    const u32 size)
{

    FILE *file = fopen(path, "w");
    if (!file) return SPIRIT_FAILURE;

    if(fwrite(path, size, 1, file) != size) return SPIRIT_FAILURE;
    
    if(fclose (file)) return SPIRIT_UNDEFINED;
    return SPIRIT_SUCCESS;
}

SpiritResult spCreateFolder (const char *path)
{

    db_assert(path, "Must pass a valid filepath");
    
    // localize filepath
    u32 localizerLength = spPlatformLocalizeFileName(
        NULL,
        path,
        0
    );
    char filepath[localizerLength];
    spPlatformLocalizeFileName(
        filepath,
        path,
        localizerLength
    );

    mkdir (filepath, S_IRWXU);
}