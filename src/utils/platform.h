#pragma once
#include <spirit_header.h>

// Platform specific systems
//
//
// Kael Johnston May 1 2022
#pragma once
#include <spirit_header.h>

/**
 * This file contains platform specific file utiltities, like creating folders,
 * getting file sizes and so on.
 *
 * It also has functionality to convert filepaths to
 * be relative to the folder with the executable file is, not the folder the program
 * was run from. This requires the user to set the executable directory using a call to
 * spPlatformSetExecutableFolder(argv[0]). If this is not done, the program with crash.
 *
 * All filepaths passed to functions here will be automatically localized, unless
 * they start with a './', in which case it is assumed they should be relative to
 * the executable directory.
 *
 * The file localizer will also convert windows file breaks ('\') to linux file breaks
 * ('/'), to allow file paths to be cross platform. This has the side effect of
 * being able to write paths like this "\\home/user\\why/would\\you/do\\this".
 *
 * @author Kael Johnston
 * @date Jul 23 2022
 */

 // platform file denominator ('/' or '\')

#if defined(__unix) || defined(__APPLE__)
#define SPIRIT_PLATFORM_FOLDER_BREAK '/' // unix
#include <sys/queue.h>
#elif defined(_WIN32)
#define SPIRIT_PLATFORM_FOLDER_BREAK '\\'
#include "platform/platform_queue_bundle_win32.h"
#else
#error Unsupported Platform
#endif

/**
 * Get the directory the program was run from.
 * This is NOT the directory the program file is in, it is the
 * folder the command to run the executable was executed in.
 *
 * @return a string containing the current working directory.
 *
 * @author Kael Johnston
 */
char* spPlatformGetCWD(void);

/**
 * Set the folder the executable file is in. This takes the command used to
 * run the program, and uses it to figure out the relative path to the
 * program folder. This allows many functions to find assets without needing
 * paths from /home/whatever.
 *
 * @param argv0 This argument should almost always be argv[0]
 *
 * @author Kael Johnston
 */
void spPlatformSetExecutableFolder(char* argvO) SPIRIT_NONULL(1);

/**
 * Get the folder the exectuable file is stored in.
 * Do NOT modify it's contents.
 *
 * @return A string containing a relative path to the executable folder
 *
 * @author Kael Johnston
 */
const char* spPlatformGetExecutableFolder(void);

// get the length of the executable directory string
// it is faster then calling strlen, because it is stored
u32 spPlatformGetExecutableFolderStrLen(void);

/**
 * @brief Test if a file operation will be permitted. Only file operations within
 * the engine executables directory will be permitted.
 *
 * @param filepath the file to be operated on
 * @return true the operation is allowed.
 * @return false the operation will be prevented
 */
bool spPlatformIsAllowedFileOperation(const char* filepath) SPIRIT_NONULL(1);

/**
 * Convert the path argument to be relative the the executable file,
 * and to the respective OS.
 *
 * @param output Can be null, and only max will be set.
 * @param max must be a valid address. If it's value is 0, it will be set.
 * Otherwise it will be used as the max. Ensure it is initialized properly.
 * @param path The filename that will be localized. Must be a valid pointer,
 *  no matter what.
 *
 * @return SPIRIT_SUCCESS for success, SPIRIT_FAILURE for failure
 *
 * @author Kael Johnston
 */
SpiritResult spPlatformLocalizeFileName(
    char* output,
    const char* path,
    u32* max) SPIRIT_NONULL(2, 3);

/**
 * Get the system time, in seconds since 1970.
 *
 * @return the current unix time
 *
 * @author Kael Johnston
 */
time_t spPlatformGetTime(void);

/**
 * @brief Get the time the program has been running, in ticks. It may roll over
 * unexpectedly, so be carefull to check for that.
 *
 * @return u64
 */
u64 spPlatformGetRunningTime(void);

/**
 * Test if a file exists. It will automatically localize the filename,
 * like all other file utilities.
 *
 * @param filepath the folder to test for
 *
 * @return true if the folder exists, false if not
 *
 * @author Kael Johnston
 */
bool spPlatformTestForFile(const char* filepath) SPIRIT_NONULL(1);

/**
 * Test the size of a file. It will return the size of the file
 * in bytes. It will return 0 if the file does not exist, or for failure.
 *
 * It automatically localizes the filename, see spPlatformLocalizeFilename,
 * like all other file utils.
 *
 * @param filepath the file to test the size of, must be a valid string
 *
 * @result the size of the file, or 0 if it does not exist or otherwise fails.
 *
 * @author Kael Johnston
 */
size_t spPlatformTestFileSize(const char* filepath) SPIRIT_NONULL(1);

/**
 * @brief Check if a folder is empty
 *
 * @param filepath
 * @return true
 * @return false
 */
bool spPlatformIsDirectoryEmpty(char* filepath) SPIRIT_NONULL(1);

/**
 * Find the last time a file was modified. The time is stored in unix time,
 * or seconds since 1970. To use this time, look at <time.h>.
 *
 * This function returns 0 for failure, or if the file does not exist. This is ok,
 * because no computers from 1970 support vulkan, so good luck having a file from
 * that time.
 *
 * @param filepath the file to find the modified date for
 *
 * @return the last time the file was modified, in unix time
 *
 * @author Kael Johnston
 */
time_t spPlatformGetFileModifiedDate(const char* filepath) SPIRIT_NONULL(1);

/**
 * Create a new directory relative to the executable folder.
 * The file name will be automatically localized, like all other file utiltities.
 * This function also creates parent directories.
 *
 * @param filepath the folder to create. It can end in a '/', but shouldn't.
 *
 * @return SPIRIT_SUCCESS for success, and SPIRIT_FAILURE otherwise
 *
 * @author Kael Johnston
 */
SpiritResult spPlatformCreateFolder(const char* restrict filepath) SPIRIT_NONULL(1);

/**
 * @brief Delete folders contents. Be careful and do not
 * misuse this function. This operation is permanent.
 *
 * @param filepath the folder or file to delete
 * @return SpiritResult
 */
SpiritResult spPlatformCreateFolder(const char* restrict filepath) SPIRIT_NONULL(1);

/**
 * @brief Delete a file or folder. It will
 * not work on folders with contents.
 *
 * @param filepath the file to delete
 * @return SpiritResult
 */
SpiritResult spPlatformDeleteFile(const char* restrict filepath) SPIRIT_NONULL(1);

/**
 * @brief Delete an folder and its contexts. this operation is permenant
 *
 * @param filepath the folder to delete
 * @return SpiritResult
 */
SpiritResult spPlatformDeleteFolder(const char* restrict filepath) SPIRIT_NONULL(1);
