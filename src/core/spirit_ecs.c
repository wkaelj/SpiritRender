#include "spirit_ecs.h"


// create a spirit
SpiritECS spCreateECS(SpiritECSCreateInfo *createinfo)
{
    
    SpiritECS ecs = new_flex_array(
            struct t_SpiritECS, 
            SpiritComponent, 
            createinfo->componentCount);

    return ecs;
}

SpiritComponent *spECSGetComponent(
        const SpiritECS *ecs, 
        const SpiritComponentReference *ref)
{
    db_assert((*ecs)->components[ref->index].type != SPIRIT_COMPONENT_NULL, 
        "Component accessed was NULL");
    return &(*ecs)->components[ref->index];
}

size_t spECSGetAvailableComponentIndex(SpiritECS *ecs)
{
    size_t index = 0;
    while ((*ecs)->components[index].type != SPIRIT_COMPONENT_NULL)
    {
        index++;

        // if no room in the ecs
        if (index >= (*ecs)->componentCount)
        {
            (*ecs)->componentCount *= 2; // double amount of space for components
            size_t newSize = 
                sizeof(struct t_SpiritECS) + 
                sizeof(SpiritComponent) * (*ecs)->componentCount;
            log_warning("Reallocating ECS to fit more entities, size = %u",
                (*ecs)->componentCount);

            *ecs = realloc(*ecs, newSize);
        }
    }

    return index;
}

SpiritComponentReference spECSAddComponent(
    SpiritECS *ecs, 
    const SpiritComponent *component)
{
    size_t index = spECSGetAvailableComponentIndex(ecs);
    (*ecs)->components[index] = *component;
    return spECSCreateReference(component->type, index);
}
