#include "spirit_file.h"
#include <stdio.h>
#include <sys/stat.h>

// just a wrapper, to help keep the file utilities organized
u64 spReadFileSize(const char *path)
{
    // localize filepath
    u32 pathLength = 0;
    spPlatformLocalizeFileName(NULL, path, &pathLength);
    char filepath[pathLength];
    spPlatformLocalizeFileName(filepath, path, &pathLength);
    return spPlatformTestFileSize(filepath);
}

bool spReadFileExists(const char *path)
{

        // localize filepath
    u32 pathLength = 0;
    spPlatformLocalizeFileName(NULL, path, &pathLength);
    char filepath[pathLength];
    spPlatformLocalizeFileName(filepath, path, &pathLength);
    return spPlatformTestForFile(filepath);
}

SpiritResult spReadFileBinary(
    void *restrict dest,
    const char *path,
    u64 *restrict size)
{

    // localize filepath
    u32 pathLength = 0;
    spPlatformLocalizeFileName(NULL, path, &pathLength);
    char filepath[pathLength];
    spPlatformLocalizeFileName(filepath, path, &pathLength);

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
    const char *path,
    u64 *restrict length)
{

        // localize filepath
    u32 pathLength = 0;
    spPlatformLocalizeFileName(NULL, path, &pathLength);
    char filepath[pathLength];
    spPlatformLocalizeFileName(filepath, path, &pathLength);

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
        fread(dest, *length, 1, file);
        dest[*length] = '\0';
    }

    fclose(file);
    return SPIRIT_SUCCESS;
}

time_t spReadFileModifiedTime(const char *restrict path)
{

        // localize filepath
    u32 pathLength = 0;
    spPlatformLocalizeFileName(NULL, path, &pathLength);
    char filepath[pathLength];
    spPlatformLocalizeFileName(filepath, path, &pathLength);
    return spPlatformGetFileModifiedDate(filepath);
}

SpiritResult spWriteFileBinary(
    const char *path,
    const void *contents,
    const u32 size)
{
    // localize filepath
    u32 pathLength = 0;
    spPlatformLocalizeFileName(NULL, path, &pathLength);
    char filepath[pathLength];
    spPlatformLocalizeFileName(filepath, path, &pathLength);

    FILE *file = fopen(filepath, "w");
    if (!file)
        return SPIRIT_FAILURE;



    db_assert(ftell(file) == 0, "File not at start");

    if (fwrite(contents, size, 1, file) == 0 && ferror(file))
    {
        log_error("Failed to write data to file '%s'", path);
        return SPIRIT_FAILURE;
    }

    if (fclose(file))
        return SPIRIT_UNDEFINED;
    return SPIRIT_SUCCESS;
}

SpiritResult spWriteFileFolder(const char *path)
{
    // localize filepath
    u32 pathLength = 0;
    spPlatformLocalizeFileName(NULL, path, &pathLength);
    char filepath[pathLength];
    spPlatformLocalizeFileName(filepath, path, &pathLength);
    return spPlatformCreateFolder(filepath);
}