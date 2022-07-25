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
#define SPIRIT_PLATFORM_FOLDER_BREAK '/' // unix

/**
 * Get the directory the program was run from.
 * This is NOT the directory the program file is in, it is the 
 * folder the command to run the executable was executed in.
 * 
 * @return a string containing the current working directory.
 * 
 * @author Kael Johnston
 */
char *spPlatformGetCWD (void);

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
void spPlatformSetExecutableFolder (char *argvO);

/**
 * Get the folder the exectuable file is stored in.
 * Do NOT modify it's contents.
 * 
 * @return A string containing a relative path to the executable folder
 * 
 * @author Kael Johnston
 */
const char *spPlatformGetExecutableFolder (void);

// get the length of the executable directory string
// it is faster then calling strlen, because it is stored
u32 spPlatformGetExecutableFolderStrLen (void);

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
SpiritResult spPlatformLocalizeFileName(char *output, const char *path, u32* max);

/**
 * Get the system time, in seconds since 1970.
 * 
 * @return the current unix time
 * 
 * @author Kael Johnston
 */
time_t spPlatformGetTime(void);

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
bool spPlatformTestForFile(const char *filepath);

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
size_t spPlatformTestFileSize (const char *filepath);

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
time_t spPlatformGetFileModifiedDate(const char *filepath);

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
SpiritResult spPlatformCreateFolder(const char *filepath);
