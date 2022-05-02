#include "platform_unix.h"
#include <unistd.h>
#include <time.h>


void spSetExecutableFolder (const char *name)
{
    // FIXME needs to remove executable file name from path
    u32 pathLength = spStringLen(name, 0) + 1;
    g_executableDirerctory = alloc(pathLength);
    spStringCpy (name, g_executableDirerctory, pathLength);
    return;
}

const char *spGetExecutableFolder (void)
{
    return g_executableDirerctory;
}

char *spGetCWD (void)
{
    return getcwd (NULL, 0);
}

SpiritBool spTestForFile(const char *filepath)
{
    return access (filepath, F_OK|R_OK);
}

u64 spGetUnixTime(void)
{
    return time (NULL);
}