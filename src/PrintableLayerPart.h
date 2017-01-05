#ifndef PRINTABLE_LAYER_PART_H
#define PRINTABLE_LAYER_PART_H

#include "utils/polygon.h"
#include "utils/AABB.h"
#include "sliceDataStorage.h"

namespace cura
{

/*!
 * Abstract class for representing a printable part within a layer.
 * This could be a SliceLayerPart of a mesh, a piece of support, a prime tower, etc.
 * 
 * The PrintableLayerPart knows itself how it should generate its paths.
 */
class PrintableLayerPart
{
public:
    virtual Polygons& getOutline() = 0;
    virtual AABB getBoundaryBox() = 0;
    virtual int64_t getZ() = 0;
    void generatePaths(GCodePlanner& layer_plan);
    bool isGenerated();

    unsigned int getExtruderNr();
    unsigned int getLayerNr();
protected:
    virtual void writeGcode(GCodePlanner& layer_plan) = 0;
    
    SliceDataStorage& storage;
    // TODO: maybe migrate the is_generated boolean to somewhere inside the Towering algorithm since it really is
    // more of an implementation detail of that algorithm
    bool is_generated;
    unsigned int extruder_nr;
    unsigned int layer_nr;
        
    
    PrintableLayerPart(SliceDataStorage& storage, unsigned int extruder_nr, unsigned int layer_nr)
    : storage(storage)
    , is_generated(false)
    , extruder_nr(extruder_nr)
    , layer_nr(layer_nr)
    {
    }
    
};


} // namespace cura

#endif // PRINTABLE_LAYER_PART_H