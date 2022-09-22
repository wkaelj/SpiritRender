#include "spirit_ecs.h"

struct t_SpiritComponentAction
{
    SpiritName componentType;
    LIST_ENTRY(t_SpiritComponentAction) data;
};