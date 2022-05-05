#include "platform_unix.h"
#include <unistd.h>
#include <sys/stat.h>
#include <time.h>

// store the exectutable files directory
// so that assets and other relative directories
// can be located during runtime
char *g_executableDirerctory = NULL;
u32 g_exectuableDirectoryStrLen = 0;

void spPlatformSetExecutableFolder(char *name)
{
    spStringTruncate(name, '/'); // remove exectuable name from path
    u32 pathLength = spStringLen(name, 0) + 1;
    g_executableDirerctory = alloc(pathLength);
    spStringCpy(name, g_executableDirerctory, pathLength);
    g_exectuableDirectoryStrLen = spStringLen(name, 0);
    return;
}

const char *spPlatformGetExecutableFolder(void)
{
    return g_executableDirerctory;
}

u32 spPlatformGetExecutableFolderStrLen(void)
{
    return g_exectuableDirectoryStrLen;
}

u32 spPlatformLocalizeFileName(char *output, const char *path, const u32 max)
{

    const u32 pathLength = spStringLen(path, 0);

    // handle paths that have been set to be non relative
    if (
        path[0] == '/' ||
        path[0] == '\\' ||
        path[0] == '.')
    {
        if (max == 0 || !output)
            return pathLength;
        spStringCpy(path, output, max);
        for (u32 i = 0; output[i] != '\0'; i++)
        {
            if (output[i] == '\\')
                output[i] = SPIRIT_PLATFORM_FOLDER_BREAK;
        }
        return pathLength;
    }

    // catch cases
    if (max == 0)
        return pathLength + g_exectuableDirectoryStrLen;
    if (pathLength + g_exectuableDirectoryStrLen + 1 < max)
        return 0;

    if (output)
    {
        npf_snprintf(output, max, "%s%s", g_executableDirerctory, path);
    }

    // convert windows filepaths to unix systems
    // idk if this is enough i don't use windows
    // im sure there is more code that that will also break
    for (u32 i = 0; output[i] != '\0'; i++)
    {
        if (output[i] == '\\')
            output[i] = SPIRIT_PLATFORM_FOLDER_BREAK;
    }

    return g_exectuableDirectoryStrLen + pathLength;
}

char *spPlatformGetCWD(void)
{
    return getcwd(NULL, 0);
}

SpiritBool spPlatformTestForFile(const char *filepath)
{
    return access(filepath, F_OK | R_OK);
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