#pragma once
#include <spirit_header.h>

// store the exectutable files directory
// so that assets and other relative directories
// can be located during runtime
char *g_executableDirerctory = NULL;

char *spGetCWD (void);

// set the folder that the program executable is stored in
// should be the first argument of the main function strings
void spSetExecutableFolder (const char *name);

// get the folder the program executable is stored in
// includes trailing '/' character
const char *spGetExecutableFolder (void);

u64 spGetUnixTime (void);

// test if a file exists, and can be read
SpiritBool spTestForFile (const char *filepath);