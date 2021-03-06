#include "platform_unix.h"
#include <unistd.h>
#include <sys/stat.h>
#include <time.h>
#include <errno.h>
#include <stdio.h>

// store the exectutable files directory
// so that assets and other relative directories
// can be located during runtime
char *g_executableDirectory = NULL;
u32 g_executableDirectoryLength = 0;

void spPlatformSetExecutableFolder(char *name)
{

    // remove executable fild name from the path
    u32 pathLength = 0;
    spStringTruncate(NULL, &pathLength, name, '/', true);
    g_executableDirectory = malloc(pathLength);
    spStringTruncate(g_executableDirectory, &pathLength, name, '/', true);

    g_executableDirectoryLength = --pathLength;
    db_assert(g_executableDirectoryLength == strlen(g_executableDirectory), "");
}

const char *spPlatformGetExecutableFolder(void)
{
    return g_executableDirectory;
}

u32 spPlatformGetExecutableFolderStrLen(void)
{
    return g_executableDirectoryLength;
}

SpiritResult spPlatformLocalizeFileName(char *output, const char *path, u32 *max)
{

    // ensure the exectuable directory has been set
    if (!g_executableDirectory) return SPIRIT_FAILURE;

    db_assert(path, "'path' cannot be NULL");
    db_assert(max, "'max' cannot be NULL");
    if (!(path && max)) return SPIRIT_FAILURE; // max and path must not be null

    const u32 pathLength = strlen(path);

    // handle paths that have been set to be non relative
    if (
        path[0] == '/' ||
        path[0] == '\\' || // win thingy
        path[0] == '.')
    {
        if (*max == 0 || !output)
        {
            *max = pathLength;
            return SPIRIT_SUCCESS;
        }
        strncpy (output, path, *max);
        for (u32 i = 0; i < *max && output[i] != '\0'; i++)
        {
            if (output[i] == '\\')
                output[i] = SPIRIT_PLATFORM_FOLDER_BREAK;
        }
        *max = pathLength;
        return SPIRIT_SUCCESS;
    }

    // set max if max was not set by user
    if (*max == 0)
        *max = pathLength + g_executableDirectoryLength + 1;

    // if max is too small, return failure
    if (*max < pathLength + g_executableDirectoryLength + 1)
        return SPIRIT_FAILURE;

    // if a output location was set, use it
    if (output)
    {
        strncpy(output, g_executableDirectory, *max);
        strncat(output, path, *max - strlen(output));
    }

    // convert windows filepaths to unix systems
    // idk if this is enough i don't use windows
    // im sure there is more code that that will also break
    for (u32 i = 0; output && i < *max && output[i] != '\0'; i++)
    {
        if (output[i] == '\\') output[i] = SPIRIT_PLATFORM_FOLDER_BREAK;
    }

    output && (output[*max - 1] = '\0');

    return SPIRIT_SUCCESS;
}

char *spPlatformGetCWD(void)
{
    return getcwd(NULL, 0);
}

bool spPlatformTestForFile(const char *filepath)
{

    u32 pathLength = 0;
    spPlatformLocalizeFileName(NULL, filepath, &pathLength);
    char path[pathLength];
    spPlatformLocalizeFileName(path, filepath, &pathLength);

    if ((unsigned) access(path, F_OK | R_OK)) return false;
    return true;
}

u64 spPlatformTestFileSize(const char *filepath)
{

    u32 pathLength = 0;
    spPlatformLocalizeFileName(NULL, filepath, &pathLength);
    char path[pathLength];
    spPlatformLocalizeFileName(path, filepath, &pathLength);

    struct stat data = (struct stat) {};
    if (stat(path, &data) == -1)
    {
        if (errno == EEXIST) return 0;
        log_perror("stat('%s') failed", filepath);
        return 1;
    }

    return data.st_size;
}

time_t spPlatformGetTime(void)
{
    return time(NULL);
}

time_t spPlatformGetFileModifiedDate(const char *filepath)
{
    db_assert(filepath, "Must have valid filepath");
    if(!filepath) return 0;

    u32 pathLength = 0;
    spPlatformLocalizeFileName(NULL, filepath, &pathLength);
    char path[pathLength];
    spPlatformLocalizeFileName(path, filepath, &pathLength);

    struct stat data;
    if(stat(path, &data) == -1)
    {
        if (errno == EEXIST) return 0;
        log_perror("stat('%s') failed", filepath);
        return 1;
    }

    return data.st_ctim.tv_sec;

}

SpiritResult spPlatformCreateFolder(const char *filepath)
{
    db_assert(filepath, "Must pass a valid filepath");

    u32 pathLength = 0;
    spPlatformLocalizeFileName(NULL, filepath, &pathLength);
    char path[pathLength];
    spPlatformLocalizeFileName(path, filepath, &pathLength);

    if (path[pathLength - 1] == SPIRIT_PLATFORM_FOLDER_BREAK)
        path[pathLength - 1] = '\0';

    for (char *p = path; *p != '\0'; p++)
        if (*p == SPIRIT_PLATFORM_FOLDER_BREAK)
        {
            *p = '\0';
            if (mkdir(filepath, S_IRWXU) && errno != EEXIST)
            {
                log_perror("Error creating file");
                return SPIRIT_FAILURE;
            }

            *p = SPIRIT_PLATFORM_FOLDER_BREAK;
        }

    if (mkdir(filepath, S_IRWXU) && errno != EEXIST)
    {
        log_perror("Failed to create file");
        return SPIRIT_FAILURE;
    }

    return SPIRIT_SUCCESS;
}