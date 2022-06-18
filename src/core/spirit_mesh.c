#include "spirit_mesh.h"


// checkout a new reference to a mesh
extern const SpiritMeshReference spCheckoutMesh(
    const SpiritMeshReference meshReference)
{
    return (const SpiritMeshReference) {}; 
}

// release a reference to a mesh
extern const SpiritResult spReleaseMesh(
    const SpiritMeshReference meshReference)
{
    return SPIRIT_SUCCESS;
}
