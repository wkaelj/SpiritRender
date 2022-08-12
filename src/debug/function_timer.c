#include "function_timer.h"

#include <stdio.h>

#include <utils/spirit_string.h>
#include <utils/platform.h>

#define LINE_LENGTH_BUFFER 512
#define FUNCTION_LENGTH_BUFFER 128
#define FUNCTION_TIMER_CACHE "function_timer.csv"

//
// Types
//

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
    struct TimerData timerData;
    LIST_ENTRY(FunctionListNode_t) data;
};

struct {
    char functionName[FUNCTION_LENGTH_BUFFER];
    u64 startTime;
    bool running;
    bool initialized;

    LIST_HEAD(FunctionListHead_t, FunctionListNode_t) functions;
    
} g_timerData = {};

//
// Helper Declerations
//

// update avg, min and max for an existing element
void addDataPoint(struct FunctionListNode_t *node, u64 executionTime);

// add a new element to the list
void addNewElement(const char *name, u64 executionTime);

// check to see if a node has been created, if so get a reference
struct FunctionListNode_t *findFunctionData(const char *functionName);

// export the linked list to a file
void exportListToFile(struct FunctionListHead_t *listHead, const char *restrict file);

// convert a TimerData into a string for a .csv file
void createTimerString(char *restrict buf, struct TimerData data);

// convert the clock_t time data to ms
f64 clockToMs(clock_t time);

//
// Public methods
//

void init_timer(void)
{
        
    LIST_INIT(&g_timerData.functions);

    g_timerData.initialized = true;
}

void terminate_timer(void)
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

void start_timer(const char *func)
{

    if (!g_timerData.initialized)
    {
        printf("function_timer: Timer has not been initialized\n");
        return;
    }

    if (g_timerData.running)
    {
        printf ("function_timer: Will not start timer, timer is running\n");
        return;
    }

    u32 funcNameLength = FUNCTION_LENGTH_BUFFER;

    // truncate and set name
    spStringTruncate(
        g_timerData.functionName,
        &funcNameLength,
        func,
        '(',
        false);
    
    // get time
    g_timerData.startTime = spPlatformGetRunningTime();

    g_timerData.running = true;
}

void end_timer(void)
{

    if (!g_timerData.initialized)
    {
        printf("function_timer: Timer has not been initialized\n");
        return;
    }

    if (!g_timerData.running)
    {
        printf("function_timer: Cannot end timer, timer is not running\n");
        return;
    }

    // get time clock ticks
    u64 endTime = spPlatformGetRunningTime();
    if (endTime < g_timerData.startTime)
    {
        printf("function_timer: Clock rollover detected, discaring measurement\n");
        g_timerData.running = false;
        return;
    }

    const u64 executionTime = endTime - g_timerData.startTime;

    
    struct FunctionListNode_t *node = findFunctionData(g_timerData.functionName);

    if (node)
    {
        addDataPoint(node, executionTime);
    } else
    {
        addNewElement(g_timerData.functionName, executionTime);
    }

    g_timerData.startTime = 0;
    g_timerData.running = false;
}

//
// Helper implementations
//

void createTimerString(char *restrict buf, struct TimerData data)
{
    // func,avgTime,maxTime,minTime, ,totalExecutionCount,totalExecutionTime
    snprintf(buf, LINE_LENGTH_BUFFER, "%s,%u,%u,%u\n",
        data.functionName,
        data.avgTime,
        data.maxTime,
        data.minTime);
}

void addDataPoint(struct FunctionListNode_t *node, u64 executionTime)
{
    struct TimerData *data = &node->timerData;

    data->totalExecutionTime += executionTime;
    data->totalExecutionCount++;
    data->avgTime = data->totalExecutionTime / data->totalExecutionCount;
    data->maxTime = max_value(data->maxTime, executionTime);
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
    strncpy(node->timerData.functionName, name, FUNCTION_LENGTH_BUFFER);

    LIST_INSERT_HEAD(&g_timerData.functions, node, data);
}

struct FunctionListNode_t *findFunctionData(const char *functionName)
{
    // search list for element
    struct FunctionListNode_t *np = NULL;
    LIST_FOREACH(np, &g_timerData.functions, data)
    {
        if (strncmp(np->timerData.functionName, functionName, FUNCTION_LENGTH_BUFFER) == 0)
        {
            return np;
        }
    }

    return NULL;
}

// export the linked list to a file
void exportListToFile(struct FunctionListHead_t *listHead, const char *restrict filepath)
{

    u32 pathLength = 0;
    spPlatformLocalizeFileName(NULL, filepath, &pathLength);
    char path[pathLength];
    spPlatformLocalizeFileName(path, filepath, &pathLength);

    FILE *file = fopen(path, "w");
    fprintf(file, "Function Name,Avg Time(CPU ticks),Max Time, Min Time\n");

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
    ftime *= 1000.0f;
    ftime /= CLOCKS_PER_SEC;
}