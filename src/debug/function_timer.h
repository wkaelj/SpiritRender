#pragma once

/**
 * @file function_timer.h
 * @author Kael Johnston
 * @brief Used to time a functions performance. It will write the times to a
 * .csv spreadsheet, in the format Function Name | Avg Time | Max time | Min
 * time Extra data is stored on each row, Total Time and Test Count. It can be
 * disabled by defining FUNCTION_TIMER_NO_DIAGNOSTIC
 * @version 0.1
 * @date 2022-08-07
 *
 * @copyright Copyright (c) 2022
 *
 */

/**
 * @brief time the function named call
 *
 */
#ifndef FUNCTION_TIMER_NO_DIAGNOSTIC
#define time_function(call)                              \
    do                                                   \
    {                                                    \
        struct FunctionTimerData t = start_timer(#call); \
        call;                                            \
        end_timer(t);                                    \
    } while (0)
#else
#define time_function(call) \
    do                      \
    {                       \
        call;               \
    } while (0)
#endif

/**
 * @brief time a function, and set variable to be the return of the function
 *
 */
#ifndef FUNCTION_TIMER_NO_DIAGNOSTIC
#define time_function_with_return(function, variable)        \
    do                                                       \
    {                                                        \
        struct FunctionTimerData t = start_timer(#function); \
        variable                   = function;               \
        end_timer(t);                                        \
    } while (0)
#else
#define time_function_with_return(function, variable) \
    do                                                \
    {                                                 \
        variable = function;                          \
    } while (0)
#endif

#ifndef FUNCTION_TIMER_NO_DIAGNOSTIC

struct FunctionTimerData
{
    unsigned long long startTime;
    char functionName[128];
};

/**
 * @brief Initialize the function timer
 *
 */
void init_timer(void);

/**
 * @brief terminate the function timer
 *
 */
void terminate_timer(void);

/**
 * @brief Start the timer. Should be called right before the timed function.
 * This also truncates the string, so functionName(args) becomes functionName.
 *
 * @param func the name of the function being timed.
 */
struct FunctionTimerData start_timer(const char *func);

/**
 * @brief End the function timer and store data. It should be called right after
 * the end of the function timer
 *
 * @param t the object returned by start_timer, to calulate duration
 *
 */
void end_timer(struct FunctionTimerData t);

#endif
