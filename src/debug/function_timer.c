#include "function_timer.h"

#include <stdio.h>

<<<<<<< HEAD
#include <utils/spirit_string.h>
#include <utils/platform.h>
=======
#include <utils/platform.h>
#include <utils/spirit_string.h>
>>>>>>> devel

#define LINE_LENGTH_BUFFER 512
#define FUNCTION_LENGTH_BUFFER 128
#define FUNCTION_TIMER_CACHE "function_timer.csv"

//
// Types
//

<<<<<<< HEAD
struct TimerData {
    char functionName[FUNCTION_LENGTH_BUFFER];
    clock_t avgTime;
    clock_t maxTime;
    clock_t minTime;
    // remember blank
    u64 totalExecutionCount; // used for average
    clock_t totalExecutionTime; // used for average
};


struct FunctionListNode_t {
=======
#ifndef FUNCTION_TIMER_NO_DIAGNOSTIC

struct TimerData
{
    char functionName[FUNCTION_LENGTH_BUFFER];
    f64 avgTime;
    f64 maxTime;
    f64 minTime;
    // remember blank
    u64 totalExecutionCount; // used for average
    f64 totalExecutionTime;  // used for average
};

struct FunctionListNode_t
{
>>>>>>> devel
    struct TimerData timerData;
    LIST_ENTRY(FunctionListNode_t) data;
};

<<<<<<< HEAD
struct {
=======
struct
{
>>>>>>> devel
    bool initialized;

    LIST_HEAD(FunctionListHead_t, FunctionListNode_t) functions;

} g_timerData = {};

//
// Helper Declerations
//

// update avg, min and max for an existing element
<<<<<<< HEAD
void addDataPoint(struct FunctionListNode_t *node, clock_t executionTime);

// add a new element to the list
void addNewElement(const char *name, u64 executionTime);
=======
void addDataPoint(struct FunctionListNode_t *node, f64 executionTime);

// add a new element to the list
void addNewElement(const char *name, f64 executionTime);
>>>>>>> devel

// check to see if a node has been created, if so get a reference
struct FunctionListNode_t *findFunctionData(const char *functionName);

// export the linked list to a file
<<<<<<< HEAD
void exportListToFile(struct FunctionListHead_t *listHead, const char *restrict file);
=======
void exportListToFile(
    struct FunctionListHead_t *listHead, const char *restrict file);
>>>>>>> devel

// convert a TimerData into a string for a .csv file
void createTimerString(char *restrict buf, struct TimerData data);

// convert the clock_t time data to ms
f64 clockToMs(clock_t time);

//
// Public methods
//

<<<<<<< HEAD
 void __attribute__((constructor)) init_timer(void)
=======
void __attribute__((constructor)) init_timer(void)
>>>>>>> devel
{

    LIST_INIT(&g_timerData.functions);

    g_timerData.initialized = true;
}

void __attribute__((destructor)) terminate_timer(void)
{
    // TODO

    // write data to file
    exportListToFile(&g_timerData.functions, FUNCTION_TIMER_CACHE);

    // delete list and free memory
    struct FunctionListNode_t *np;
    while (!LIST_EMPTY(&g_timerData.functions))
    {
        np = LIST_FIRST(&g_timerData.functions);
        LIST_REMOVE(np, data);
        free(np);
    };

    g_timerData.initialized = false;
}

struct FunctionTimerData start_timer(const char *func)
{

    if (!g_timerData.initialized)
    {
        printf("function_timer: Timer has not been initialized\n");
<<<<<<< HEAD
        return (struct FunctionTimerData) {0};
=======
        return (struct FunctionTimerData){0};
>>>>>>> devel
    }

    struct FunctionTimerData t = {};

    u32 funcNameLength = FUNCTION_LENGTH_BUFFER;

    // truncate and set name
<<<<<<< HEAD
    spStringTruncate(
        t.functionName,
        &funcNameLength,
        func,
        '(',
        false);
=======
    spStringTruncate(t.functionName, &funcNameLength, func, '(', false);
>>>>>>> devel

    // get time
    t.startTime = spPlatformGetRunningTime();

    return t;
<<<<<<< HEAD

=======
>>>>>>> devel
}

void end_timer(struct FunctionTimerData t)
{

    if (!g_timerData.initialized)
    {
        printf("function_timer: Timer has not been initialized\n");
        return;
    }

    // get time clock ticks
    u64 endTime = spPlatformGetRunningTime();
    if (endTime < t.startTime)
    {
<<<<<<< HEAD
        printf("function_timer: Clock rollover detected, discaring measurement\n");
        return;
    }

    const u64 executionTime = endTime - t.startTime;

=======
        printf(
            "function_timer: Clock rollover detected, discaring measurement\n");
        return;
    }

    const f64 executionTime = clockToMs(endTime - t.startTime);
>>>>>>> devel

    struct FunctionListNode_t *node = findFunctionData(t.functionName);

    if (node)
    {
        addDataPoint(node, executionTime);
<<<<<<< HEAD
    } else
=======
    }
    else
>>>>>>> devel
    {
        addNewElement(t.functionName, executionTime);
    }
}

//
// Helper implementations
//

void createTimerString(char *restrict buf, struct TimerData data)
{
    // func,avgTime,maxTime,minTime, ,totalExecutionCount,totalExecutionTime
<<<<<<< HEAD
    snprintf(buf, LINE_LENGTH_BUFFER, "%s,%lu,%lu,%lu\n",
=======
    snprintf(
        buf,
        LINE_LENGTH_BUFFER,
        "%s,%fms,%fms,%fms\n",
>>>>>>> devel
        data.functionName,
        data.avgTime,
        data.maxTime,
        data.minTime);
}

<<<<<<< HEAD
void addDataPoint(struct FunctionListNode_t *node, clock_t executionTime)
=======
void addDataPoint(struct FunctionListNode_t *node, f64 executionTime)
>>>>>>> devel
{
    struct TimerData *data = &node->timerData;

    data->totalExecutionTime += executionTime;
    data->totalExecutionCount++;
    data->avgTime = data->totalExecutionTime / data->totalExecutionCount;
    data->maxTime = max_value(data->maxTime, executionTime);
<<<<<<< HEAD
    data->minTime =
        data->minTime != 0 ? min_value(data->minTime, executionTime) : executionTime;
}

// add a new element to the list
void addNewElement(const char *name, u64 executionTime)
{
    struct FunctionListNode_t *node = new_var(struct FunctionListNode_t);
    node->timerData = (struct TimerData) {
        .functionName = {},
        .avgTime = executionTime,
        .maxTime = executionTime,
        .minTime = executionTime,
        .totalExecutionCount = 1,
        .totalExecutionTime = executionTime
    };
=======
    data->minTime = data->minTime != 0 ? min_value(data->minTime, executionTime)
                                       : executionTime;
}

// add a new element to the list
void addNewElement(const char *name, f64 executionTime)
{
    struct FunctionListNode_t *node = new_var(struct FunctionListNode_t);
    node->timerData                 = (struct TimerData){
                        .functionName        = {},
                        .avgTime             = executionTime,
                        .maxTime             = executionTime,
                        .minTime             = executionTime,
                        .totalExecutionCount = 1,
                        .totalExecutionTime  = executionTime};
>>>>>>> devel
    strncpy(node->timerData.functionName, name, FUNCTION_LENGTH_BUFFER);

    LIST_INSERT_HEAD(&g_timerData.functions, node, data);
}

struct FunctionListNode_t *findFunctionData(const char *functionName)
{
    // search list for element
    struct FunctionListNode_t *np = NULL;
    LIST_FOREACH(np, &g_timerData.functions, data)
    {
<<<<<<< HEAD
        if (strncmp(np->timerData.functionName, functionName, FUNCTION_LENGTH_BUFFER) == 0)
=======
        if (strncmp(
                np->timerData.functionName,
                functionName,
                FUNCTION_LENGTH_BUFFER) == 0)
>>>>>>> devel
        {
            return np;
        }
    }

    return NULL;
}

// export the linked list to a file
<<<<<<< HEAD
void exportListToFile(struct FunctionListHead_t *listHead, const char *restrict filepath)
=======
void exportListToFile(
    struct FunctionListHead_t *listHead, const char *restrict filepath)
>>>>>>> devel
{

    u32 pathLength = 0;
    spPlatformLocalizeFileName(NULL, filepath, &pathLength);
    char path[pathLength];
    spPlatformLocalizeFileName(path, filepath, &pathLength);

    FILE *file = fopen(path, "w");
<<<<<<< HEAD
    fprintf(file, "Function Name,Avg Time(CPU ticks),Max Time, Min Time\n");
=======
    fprintf(file, "Function Name,Avg Time(ms),Max Time(ms), Min Time(ms)\n");
>>>>>>> devel

    printf("Data:");
    // write each element to string
    char lineBuf[LINE_LENGTH_BUFFER];
    struct FunctionListNode_t *np = NULL;
    LIST_FOREACH(np, listHead, data)
    {
        createTimerString(lineBuf, np->timerData);
        fputs(lineBuf, file);
        printf("\t%s", lineBuf);
    }

    fclose(file);
}

f64 clockToMs(clock_t time)
{

    f64 ftime = time;
<<<<<<< HEAD
    ftime *= 1000.0f;
    ftime /= CLOCKS_PER_SEC;

    return ftime;
}
=======
    ftime *= 1000.0;
    ftime /= CLOCKS_PER_SEC;

    return ftime;
}

#endif
>>>>>>> devel
