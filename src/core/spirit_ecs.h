/**
 * @file spirit_ecs.h
 * @author Kael Johnston (wkaelj@gmail.com)
 * @brief Load, create, and manage components
 * @version 0.1
 * @date 2022-09-16
 *
 * @copyright Copyright (c) 2022
 *
 */

#include <spirit_header.h>

#include <utils/map.h>
#include <utils/spirit_vector.h>

typedef struct t_SpiritComponentType
{

    SpiritName name;
    size_t componentSize; // the size of the component struct

    DVECTOR components;

    int (*init_fn)(void *self);
    int (*destroy_fn)(void *self);
    int (*update_fn)(void *self, float delta);

    // Store the component dll, to be freed. Can be null for built in components
    void *component_lib;
    bool unloadQueued;
} SpiritComponentType;

typedef enum e_SpiritComponentActionType
{
    SPIRIT_COMPONENT_UNLOAD = 0,
    SPIRIT_COMPONENT_RELOAD,
} SpiritComponentActionType;

typedef struct t_SpiritComponentAction SpiritComponentAction;

typedef struct t_SpiritComponentManager
{
    hashmap *components; // key: int | value: SpiritComponentType
    LIST_HEAD(SpiritActionQueueListHead, SpiritComponentAction)
        actions; // TODO queue
} SpiritComponentManager;

SpiritResult spComponentLoad(SpiritComponentManager *cm, const char *path);

SpiritResult spComponentQueueAction(
    SpiritComponentManager *cm,
    SpiritName component,
    SpiritComponentActionType action);