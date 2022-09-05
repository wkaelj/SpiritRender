#include "platform_unix.h"
<<<<<<< HEAD
#include <sys/cdefs.h>
#include <unistd.h>
#include <sys/stat.h>
#include <time.h>
#include <errno.h>
#include <stdio.h>
#include <dirent.h>
#include <ftw.h>
=======
#include <dirent.h>
#include <errno.h>
#include <ftw.h>
#include <stdio.h>
#include <sys/cdefs.h>
#include <sys/stat.h>
#include <time.h>
#include <unistd.h>
>>>>>>> devel

/**
 * @brief Localize a file name. This macro is more convientent then writing the
 * same code over and over again.
 *
 * @param inputpath the variable storing the path to localize
 * @param outputpathname the name of the localized path name. the macro
 * creates it.
 * @param outputpathlength the name of the variable storing the localized
 * filepaths length. the macro creates it
 *
 */
<<<<<<< HEAD
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
=======
#define localize_path(inputPath, outputPathName, outputPathLength)  \
    u32 outputPathLength = 0;                                       \
    spPlatformLocalizeFileName(NULL, inputPath, &outputPathLength); \
    char outputPathName[outputPathLength];                          \
    spPlatformLocalizeFileName(outputPathName, inputPath, &outputPathLength);

#define assert_executable_directory_set()                                      \
    if (g_executableDirectory == NULL)                                         \
    {                                                                          \
        log_fatal(                                                             \
            "The executable directory has not been set. The SpiritRender\n"    \
            "requires that the executable directory is set using\n"            \
            "spPlatformSetExecutableDirectory(argv[0] in order to verify\n"    \
            "that file operations are safe, for the developer and the user."); \
        abort();                                                               \
    }

#define assert_allowed_file_operation(path)      \
    if (!spPlatformIsAllowedFileOperation(path)) \
        abort();
>>>>>>> devel

// store the exectutable files directory
// so that assets and other relative directories
// can be located during runtime
char g_executableDirectory[1024];
u32 g_executableDirectoryLength = 0;

bool spPlatformIsAllowedFileOperation(const char *filepath)
{
<<<<<<< HEAD

    const char *fileErr = "Illigal file operation. A file operation is "
        "being made outsidethe project directory.";
=======
    const char *fileErr = "Illigal file operation. A file operation is "
                          "being made outsidethe project directory.";
>>>>>>> devel

    db_assert_msg(strlen(filepath) > g_executableDirectoryLength, "");

    char *match = strstr(filepath, g_executableDirectory);
<<<<<<< HEAD
    if (match == filepath && strlen(filepath) > 0) return true;
=======
    if (match == filepath && strlen(filepath) > 0)
        return true;
>>>>>>> devel
    else
    {
        log_fatal("%s", fileErr);
        return false;
    }
}

void spPlatformSetExecutableFolder(char *name)
{
<<<<<<< HEAD

    // remove executable fild name from the path
    u32 pathLength = 0;
    spStringTruncate(NULL, &pathLength, name, '/', true);
    db_assert_msg(pathLength < sizeof(g_executableDirectory), "Executable path too long");
    g_executableDirectoryLength = --pathLength;
    pathLength = sizeof(g_executableDirectory);
    spStringTruncate(g_executableDirectory, &pathLength, name, '/', true);

    db_assert_msg(g_executableDirectoryLength == strlen(g_executableDirectory), "");
=======
    // remove executable fild name from the path
    u32 pathLength = 0;
    spStringTruncate(NULL, &pathLength, name, '/', true);
    db_assert_msg(
        pathLength < sizeof(g_executableDirectory), "Executable path too long");
    g_executableDirectoryLength = --pathLength;
    pathLength                  = sizeof(g_executableDirectory);
    spStringTruncate(g_executableDirectory, &pathLength, name, '/', true);

    db_assert_msg(
        g_executableDirectoryLength == strlen(g_executableDirectory), "");
>>>>>>> devel
}

const char *spPlatformGetExecutableFolder(void)
{
    return g_executableDirectory;
}

u32 spPlatformGetExecutableFolderStrLen(void)
{
    return g_executableDirectoryLength;
}

SpiritResult spPlatformLocalizeFileName(
    char *output, const char *path, u32 *max)
{
<<<<<<< HEAD

=======
>>>>>>> devel
    const u32 pathLength = strlen(path);

    // handle paths that have been set to be non relative
    if (path[0] == '/' || path[0] == '\\' || // win thingy
        path[0] == '.')
    {
        if (*max == 0 || !output)
        {
            *max = pathLength;
            return SPIRIT_SUCCESS;
        }
<<<<<<< HEAD
        strncpy (output, path, *max);
=======
        strncpy(output, path, *max);
>>>>>>> devel
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
<<<<<<< HEAD

    // if max is too small, return failure
    if (*max < pathLength + g_executableDirectoryLength + 1)
        return SPIRIT_FAILURE;

=======

    // if max is too small, return failure
    if (*max < pathLength + g_executableDirectoryLength + 1)
        return SPIRIT_FAILURE;

>>>>>>> devel
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
<<<<<<< HEAD

    return SPIRIT_SUCCESS;
}
=======
>>>>>>> devel

    return SPIRIT_SUCCESS;
}

<<<<<<< HEAD
bool spPlatformTestForFile(const char *filepath)
{

=======
char *spPlatformGetCWD(void) { return getcwd(NULL, 0); }

bool spPlatformTestForFile(const char *filepath)
{
>>>>>>> devel
    localize_path(filepath, path, pathLength);

    if (access(path, F_OK | R_OK) == -1)
    {
        log_perror("Failed to test for file '%s'", path);
        return false;
    }
    return true;
}

u64 spPlatformTestFileSize(const char *filepath)
{
<<<<<<< HEAD

    localize_path(filepath, path, pathLength);

    struct stat data = (struct stat) {};
    if (stat(path, &data) == -1)
    {
        if (errno == EEXIST) return 0;
=======
    localize_path(filepath, path, pathLength);

    struct stat data = (struct stat){};
    if (stat(path, &data) == -1)
    {
        if (errno == EEXIST)
            return 0;
>>>>>>> devel
        log_perror("stat('%s') failed", filepath);
        return 1;
    }

    return data.st_size;
}

time_t spPlatformGetTime(void) { return time(NULL); }

u64 spPlatformGetRunningTime(void)
{
    clock_t time = clock();
    return time;
}

<<<<<<< HEAD
u64 spPlatformGetRunningTime(void)
{
    clock_t time = clock();
    return time;
}

time_t spPlatformGetFileModifiedDate(const char *filepath)
{
    db_assert_msg(filepath, "Must have valid filepath");

    localize_path(filepath, path, pathLength);

    struct stat data;
    if(stat(path, &data) == -1)
    {
        if (errno == EEXIST) return 0;
=======
time_t spPlatformGetFileModifiedDate(const char *filepath)
{
    localize_path(filepath, path, pathLength);

    struct stat data;
    if (stat(path, &data) == -1)
    {
        if (errno == EEXIST)
            return 0;
>>>>>>> devel
        log_perror("stat('%s') failed", filepath);
        return 1;
    }

<<<<<<< HEAD
    return data.st_ctime;

=======
    return data.st_mtim.tv_sec;
>>>>>>> devel
}

SpiritResult spPlatformCreateFolder(const char *filepath)
{
<<<<<<< HEAD
    db_assert_msg(filepath, "Must pass a valid filepath");

    localize_path(filepath, path, pathLength);

=======
    localize_path(filepath, path, pathLength);

>>>>>>> devel
    assert_allowed_file_operation(path);

    if (path[pathLength - 2] == SPIRIT_PLATFORM_FOLDER_BREAK)
        path[pathLength - 2] = '\0';

    for (char *p = path + 1; *p != '\0'; p++)
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

    if (mkdir(path, S_IRWXU) && errno != EEXIST)
<<<<<<< HEAD
    {
        log_perror("Failed to create file %s", path);
=======
    {
        log_perror("Failed to create file %s", path);
        return SPIRIT_FAILURE;
    }

    return SPIRIT_SUCCESS;
}

bool spPlatformIsDirectoryEmpty(char *filepath)
{
    localize_path(filepath, path, pathLength);

    int n = 0;
    struct dirent *d;
    DIR *dir = opendir(path);
    if (dir == NULL) // not a directory or doesn't exist
        return 1;
    while ((d = readdir(dir)) != NULL)
    {
        if (++n > 2)
            break;
    }
    closedir(dir);
    if (n <= 2) // Directory Empty
        return true;
    else
        return false;
}

int ftw_deleteFolderCallback(const char *fpath,
    const struct stat *sb __attribute_maybe_unused__, int typeflag)
{
    if (typeflag & FTW_D)
    {
        DIR *folder = opendir(fpath);
        if (readdir(folder))
        {
            return 0; // shouldn't delete folders
        }
    }

    remove(fpath);
    return 0;
}

SpiritResult spPlatformDeleteFolder(const char *restrict filepath)
{
    localize_path(filepath, path, pathLength);

    assert_allowed_file_operation(path);

    // get rid of the slash so the rest of the function works properly
    if (path[pathLength - 2] == SPIRIT_PLATFORM_FOLDER_BREAK)
        path[pathLength - 2] = '\0';

    ftw(path, &ftw_deleteFolderCallback, 256);

    if (spPlatformIsDirectoryEmpty(path))
    {
        remove(path);
        return SPIRIT_SUCCESS;
    }

    return SPIRIT_FAILURE;
}

SpiritResult spPlatformDeleteFile(const char *restrict filepath)
{
    localize_path(filepath, path, pathLength);

    assert_allowed_file_operation(path);

    // check to be sure folders are empty
    struct stat file_stats;
    if (stat(path, &file_stats) == -1)
    {
        errno != EEXIST &&log_perror("%s", path);
>>>>>>> devel
        return SPIRIT_FAILURE;
    }
    if (!S_ISREG(file_stats.st_mode))
    {
        if (!spPlatformIsDirectoryEmpty(path))
        {
            log_warning("Cannot delete file that is not empty");
            return SPIRIT_FAILURE;
        }
    }

    if (remove(path) == -1)
    {
        log_perror("%s", path);
        return SPIRIT_FAILURE;
    }
    return SPIRIT_SUCCESS;
}

bool spPlatformIsDirectoryEmpty(char *filepath)
{
    localize_path(filepath, path, pathLength);

    int n = 0;
    struct dirent *d;
    DIR *dir = opendir(path);
    if (dir == NULL) // not a directory or doesn't exist
        return 1;
    while ((d = readdir(dir)) != NULL) {
        if(++n > 2)
        break;
    }
    closedir(dir);
    if (n <= 2) //Directory Empty
        return true;
    else
        return false;
}

int ftw_deleteFolderCallback(
    const char *fpath,
    const struct stat *sb __attribute_maybe_unused__,
    int typeflag)
{
    if (typeflag & FTW_D)
    {
        DIR *folder = opendir(fpath);
        if (readdir(folder))
        {
            return 0; // shouldn't delete folders
        }
    }

    remove(fpath);
    return 0;
}

SpiritResult spPlatformDeleteFolder(const char *restrict filepath)
{
    localize_path(filepath, path, pathLength);

    assert_allowed_file_operation(path);

    // get rid of the slash so the rest of the function works properly
    if (path[pathLength - 2] == SPIRIT_PLATFORM_FOLDER_BREAK)
        path[pathLength - 2] = '\0';

    ftw(path, &ftw_deleteFolderCallback, 256);

    if (spPlatformIsDirectoryEmpty(path))
    {
        remove(path);
        return SPIRIT_SUCCESS;
    }

    return SPIRIT_FAILURE;
}

SpiritResult spPlatformDeleteFile(const char *restrict filepath)
{
    localize_path(filepath, path, pathLength);

    assert_allowed_file_operation(path);

    // check to be sure folders are empty
    struct stat file_stats;
    if (stat(path, &file_stats) == -1)
    {
        errno != EEXIST && log_perror("%s", path);
        return SPIRIT_FAILURE;
    }
    if (!S_ISREG(file_stats.st_mode))
    {
        if (!spPlatformIsDirectoryEmpty(path))
        {
            log_warning("Cannot delete file that is not empty");
            return SPIRIT_FAILURE;
        }
    }

    if (remove(path) == -1)
    {
        log_perror("%s", path);
        return SPIRIT_FAILURE;
    }
    return SPIRIT_SUCCESS;
}