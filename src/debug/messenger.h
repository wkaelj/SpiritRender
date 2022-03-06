#ifndef DEBUG_H
#define DEBUG_H

#include <spirit_header.h> 

/**
 * @brief Functions to output and log debug messeges. Use the debug macros pls not the function itself
 * @author Kael Johnston
 * @date Feb 17 2022
 */


// should enable 
#define ENABLE_FATAL 1 // log a unrecoverable error
#define ENABLE_ERROR 1 // log a failure that might not crash, but behavoir is undefined
#define ENABLE_WARNING 1
#define ENABLE_VALIDATION 1
#define ENABLE_INFO 1
#define ENABLE_DEBUG 1

typedef enum {
    DEBUG_FATAL,
    DEBUG_ERROR,
    DEBUG_WARNING,
    DEBUG_VALIDATION,
    DEBUG_INFO,
    DEBUG_DEBUG,
    DEBUG_TYPE_MAX // count for use in fn
} DebugSeverity;

// check if debug messeges are enabled, otherwise disable the corrospining messege
#ifdef ENABLE_FATAL
#define LOG_FATAL(messege, ...) debug_log (DEBUG_FATAL, __FILE__, __func__, __LINE__, messege, ##__VA_ARGS__)
#else
#define LOG_FATAL(messege, ...)
#endif

// a failure within a function that may cause a crash or undefined behavior
#ifdef ENABLE_ERROR
#define LOG_ERROR(messege, ...) debug_log (DEBUG_ERROR, __FILE__, __func__, __LINE__, messege, ##__VA_ARGS__)
#else
#define LOG_ERROR(messege, ...)
#endif

// warn about improper parameters, or other trivial errors
#ifdef ENABLE_WARNING
#define LOG_WARNING(messege, ...) debug_log (DEBUG_WARNING, __FILE__, __func__, __LINE__, messege, ##__VA_ARGS__)
#else
#define LOG_WARNING(messege, ...)
#endif

// log vulkan validation layer errors
#ifdef ENABLE_VALIDATION
#define LOG_VALIDATION(messege, ...) debug_log (DEBUG_VALIDATION, __FILE__, __func__, __LINE__, messege, ##__VA_ARGS__)
#else
#define LOG_VALIDATION(messege, ...)
#endif

// log basic info, like hardware selections
#ifdef ENABLE_INFO
#define LOG_INFO(messege, ...) debug_log (DEBUG_INFO, __FILE__, __func__, __LINE__, messege, ##__VA_ARGS__)
#else
#define LOG_INFO(messege, ...)
#endif

// log debug messeges, that will be removed for final builds
#ifdef ENABLE_DEBUG
#define LOG_DEBUG(messege, ...) debug_log (DEBUG_DEBUG, __FILE__, __func__, __LINE__, messege, ##__VA_ARGS__)
#else
#define LOG_DEBUG(messege, ...)
#endif

// log debugging messege
int debug_log (DebugSeverity severity, const char* file, const char *func, const int line, const char *format, ...);

#endif
