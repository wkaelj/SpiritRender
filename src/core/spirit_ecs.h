#pragma once
#include <spirit_header.h>
//#include "components/spirit_transform_component.h"
//#include "components/spirit_mesh_render_component.h"

typedef enum t_SpiritComponentType
{
    SPIRIT_COMPONENT_NULL = 0,
    SPIRIT_COMPONENT_TRANSFORM,
    SPIRIT_COMPONENT_MESH_RENDER,

    SPIRIT_COMPONENT_MAX
} SpiritComponentType;

typedef struct t_SpiritECSCreateInfo
{
    size_t componentCount; // the amount of components supported by the ecs
} SpiritECSCreateInfo;

// a component reference is used to reference a component stored in a ecs
typedef struct t_SpiritComponentReference
{
    SpiritComponentType type;
    size_t index;
} SpiritComponentReference;

// can be used to reference any type of spirit component
typedef union t_SpiritComponent
{
    SpiritComponentType type; // allows the component to be set to NULL
    //SpiritTransformComponent transformComponent;
    //SpiritMeshRenderComponent meshRenderComponent;
} SpiritComponent;

typedef struct t_SpiritComponentListElement SpiritComponentListElement;

typedef struct t_SpiritECS
{
    size_t componentCount;
    SpiritComponent components[];
} *SpiritECS;

extern SpiritECS spCreateECS(SpiritECSCreateInfo *createinfo);

// get a pointer to the component referenced by your reference
extern SpiritComponent *spECSGetComponent(
    const SpiritECS *ecs,
    const SpiritComponentReference *reference);

extern size_t spECSGetAvailableComponentIndex(SpiritECS *ecs);

// add a component too the ecs, and get a reference too that component
extern SpiritComponentReference spECSAddComponent(
    SpiritECS *ecs,
    const SpiritComponent *component);

extern inline SpiritComponentReference spECSCreateReference(
        SpiritComponentType type,
        size_t index)
{
    return (SpiritComponentReference){type, index};
}

extern SpiritResult spDestroyECS(SpiritECS ecs);
