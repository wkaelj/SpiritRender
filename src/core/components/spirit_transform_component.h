#pragma once
#include <spirit_header.h>
#include <core/spirit_ecs.h>

// transform component
// used to store the transformation of a component
// garunteed to be on all components


typedef struct t_SpiritTransformComponent
{
    SpiritComponentType type;
    vec3 transform;
    vec3 scale;
    vec3 rotation;

    struct t_SpiritTransformComponent *parent;
} SpiritTransformComponent;

SpiritTransformComponent spInstantiateTransformComponent(
    SpiritECS *ecs,
    SpiritTransformComponent *parent,
    vec3 initialTransform);
