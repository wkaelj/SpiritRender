#include "../platform.h"

#ifdef _WIN32

#include <fileapi.h>
#include <timeapi.h>
#include <direct.h>
#include <sys/stat.h>
#include <io.h>

#include <Shlwapi.h>

#define localize_path(inputPath, outputPathName, outputPathLength) \
    u32 outputPathLength = 0; \
    spPlatformLocalizeFileName(NULL, inputPath, &outputPathLength); \
    char outputPathName[outputPathLength]; \
    spPlatformLocalizeFileName(outputPathName, inputPath, &outputPathLength);

#define assert_executable_directory_set() \
    if (g_executableDirectory == NULL) \
    { \
        log_fatal( \
            "The executable directory has not been set. The SpiritRender\n" \
            "requires that the executable directory is set using\n" \
            "spPlatformSetExecutableDirectory(argv[0] in order to verify\n" \
            "that file operations are safe, for the developer and the user."); \
        abort(); \
    }

#define assert_allowed_file_operation(path) \
    if(!spPlatformIsAllowedFileOperation(path)) abort();

char cwd[1024];
bool setcwd = false;

char executableDirectory[1024];
size_t exectuableDirectoryLength = 0;

char* spPlatformGetCWD(void)
{
	if (setcwd = false)
	{
		setcwd = true;
		_getcwd(cwd, sizeof(cwd));
	}
	return cwd;
}

void spPlatformSetExecutableFolder(char* argvO) SPIRIT_NONULL(1)
{
	if (executableDirectoryLength == 0)
	{
		exectuableDirectoryLength = strlen(argvO);
		spStringTruncate(executableDirectory, executableDirectoryLength, argvO, SPIRIT_PLATFORM_FOLDER_BREAK, false);
		exectuableDirectoryLength = strnlen(exectuableDirectoryLength, exectuableDirectoryLength);
	}
}

const char* spPlatformGetExecutableFolder(void)
{
	return exectuableDirectoryLength;
}

u32 spPlatformGetExecutableFolderStrLen(void)
{
	return (u32) exectuableDirectoryLength;
}

bool spPlatformIsAllowedFileOperation(const char* filepath)
{

    const char* fileErr = "Illigal file operation. A file operation is "
        "being made outsidethe project directory.";

    db_assert_msg(strlen(filepath) > g_executableDirectoryLength, "");

    char* match = strstr(filepath, g_executableDirectory);
    if (match == filepath && strlen(filepath) > 0) return true;
    else
    {
        log_fatal("%s", fileErr);
        return false;
    }
}

SpiritResult spPlatformLocalizeFileName(char* output, const char* path, u32* max)
{

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
        strncpy(output, path, *max);
        for (u32 i = 0; i < *max && output[i] != '\0'; i++)
        {
            if (output[i] == '/')
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
        if (output[i] == '/') output[i] = SPIRIT_PLATFORM_FOLDER_BREAK;
    }

    output && (output[*max - 1] = '\0');

    return SPIRIT_SUCCESS;
}

bool spPlatformTestForFile(const char* filepath)
{

    localize_path(filepath, path, pathLength);

    if (access(path, 0) == -1)
    {
        log_error("Failed to test for file '%s'", path);
        return false;
    }
    return true;
}

u64 spPlatformTestFileSize(const char* filepath)
{

    localize_path(filepath, path, pathLength);

    struct stat data = (struct stat){};
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

u64 spPlatformGetRunningTime(void)
{
    clock_t time = clock();
    return time;
}

time_t spPlatformGetFileModifiedDate(const char* filepath)
{
    db_assert_msg(filepath, "Must have valid filepath");

    localize_path(filepath, path, pathLength);

    struct stat data;
    if (stat(path, &data) == -1)
    {
        if (errno == EEXIST) return 0;
        log_perror("stat('%s') failed", filepath);
        return 1;
    }

    return data.st_ctime;
}

SpiritResult spPlatformCreateFolder(const char* filepath)
{
    db_assert_msg(filepath, "Must pass a valid filepath");

    localize_path(filepath, path, pathLength);

    assert_allowed_file_operation(path);

    if (path[pathLength - 2] == SPIRIT_PLATFORM_FOLDER_BREAK)
        path[pathLength - 2] = '\0';

    for (char* p = path + 1; *p != '\0'; p++)
    {
        if (*p == SPIRIT_PLATFORM_FOLDER_BREAK)
        {
            *p = '\0';
            if (mkdir(path, S_IRWXU) && errno != EEXIST)
            {
                log_perror("Error creating file %s", path);
                return SPIRIT_FAILURE;
            }

            *p = SPIRIT_PLATFORM_FOLDER_BREAK;
        }
    }

    if (mkdir(path, S_IRWXU) && errno != EEXIST)
    {
        log_perror("Failed to create file %s", path);
        return SPIRIT_FAILURE;
    }

    return SPIRIT_SUCCESS;
}

bool spPlatformIsDirectoryEmpty(char* filepath)
{
    localize_path(filepath, path, pathLength);
    LPCWSTR s;

    return PathIsDirectoryEmptyA(path);

}

#endif