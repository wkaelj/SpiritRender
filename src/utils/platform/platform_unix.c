#include "platform_unix.h"
#include <unistd.h>
#include <sys/stat.h>
#include <time.h>
#include <errno.h>

// store the exectutable files directory
// so that assets and other relative directories
// can be located during runtime
char *g_executableDirectory = NULL;
u32 g_executableDirectoryLength = 0;

void spPlatformSetExecutableFolder(char *name)
{
    u32 pathLength = 0;
    spStringTruncate(NULL, &pathLength, name, '/', true); // remove exectuable name from path
    log_info("Exectuable folder length = %u", pathLength);
    g_executableDirectory = alloc(pathLength + 1);
    spStringTruncate(g_executableDirectory, &pathLength, name, '/', true);

    g_executableDirectoryLength = pathLength;
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
    if (!(path && max)) return SPIRIT_FAILURE;

    const u32 pathLength = strlen (path);

    // handle paths that have been set to be non relative
    if (
        path[0] == '/' ||
        path[0] == '\\' || // win thingy
        path[0] == '.')
    {
        if (max == 0 || !output)
            return pathLength;
        strncpy (output, path, *max);
        for (u32 i = 0; output[i] != '\0'; i++)
        {
            if (output[i] == '\\')
                output[i] = SPIRIT_PLATFORM_FOLDER_BREAK;
        }
        return pathLength;
    }

    // catch cases
    if (*max == 0)
        *max = pathLength + g_executableDirectoryLength;
    if (pathLength + g_executableDirectoryLength + 1 < *max)
        return 0;

    if (output)
    {
        npf_snprintf(output, *max, "%s%s", g_executableDirectory, path);
    }

    // convert windows filepaths to unix systems
    // idk if this is enough i don't use windows
    // im sure there is more code that that will also break
    for (u32 i = 0; output && output[i] != '\0'; i++)
    {
        if (output[i] == '\\')
            output[i] = SPIRIT_PLATFORM_FOLDER_BREAK;
    }

    return g_executableDirectoryLength + pathLength;
}

char *spPlatformGetCWD(void)
{
    return getcwd(NULL, 0);
}

SpiritBool spPlatformTestForFile(const char *filepath)
{
    if ((unsigned) access(filepath, F_OK | R_OK)) return SPIRIT_FALSE;
    return SPIRIT_TRUE;
}

u64 spPlatformTestFileSize(const char *filepath)
{
    struct stat data;
    if (stat(filepath, &data))
        return 0;

    return data.st_size;
}

u64 spPlatformGetUnixTime(void)
{
    return time(NULL);
}

SpiritResult spPlatformCreateFolder (const char *path)
{

    db_assert(path, "Must pass a valid filepath");
    if (!path) return SPIRIT_FAILURE;
    
    const u32 pathLength = strlen (path);
    // localize filepath
    u32 localizerLength = 0;
    spPlatformLocalizeFileName(
        NULL,
        path,
        &localizerLength);
    log_debug("Localizer Length = %d", localizerLength);
    char filepath[localizerLength];
    spPlatformLocalizeFileName(
        filepath,
        path,
        &localizerLength);

    if (filepath[localizerLength - 1] == SPIRIT_PLATFORM_FOLDER_BREAK)
    {
        filepath[localizerLength] = '\0';
    }

    for (char *p = &filepath[localizerLength]; *p != '\0'; p++)
    {
        if (*p == SPIRIT_PLATFORM_FOLDER_BREAK)
        {
            *p = '\0';
            if ((unsigned) mkdir(filepath, S_IRWXU) && errno != EEXIST)
            {
                log_fatal("Error creating file '%d'", errno);
                abort();
            }

            *p = SPIRIT_PLATFORM_FOLDER_BREAK;
        }
    }

    if (mkdir(filepath, S_IRWXU))
    {
        log_error ("Failed to create final directory");
        return SPIRIT_FAILURE;
    }

    log_debug("Creating Folder '%s'", filepath);

    return SPIRIT_SUCCESS;
}