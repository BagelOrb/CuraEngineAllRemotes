#ifndef PRINTABLE_LAYER_H
#define PRINTABLE_LAYER_H

#include <vector>

#include "PrintableLayerPart.h"
#include "sliceDataStorage.h"
#include "PrintableMeshLayerPart.h"

namespace cura
{

/*!
 * Abstract class for representing all printable parts within a layer.
 * This includes SliceLayerPart of a mesh, a piece of support, a prime tower, etc.
 */
class PrintableLayer
{
public:
    std::vector<PrintableLayerPart*> parts; // has to be pointers because not all subtypes have the same size?
    int64_t getZ()
    {
        return z;
    }
    PrintableLayer(SliceDataStorage& storage, unsigned int layer_nr);
private:
    int64_t z;
};


} // namespace cura

#endif // PRINTABLE_LAYER_H