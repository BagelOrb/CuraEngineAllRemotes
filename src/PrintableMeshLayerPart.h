#ifndef PRINTABLE_MESH_LAYER_PART_H
#define PRINTABLE_MESH_LAYER_PART_H

#include "PrintableLayerPart.h"
#include "sliceDataStorage.h"

namespace cura
{

class PrintableMeshLayerPart : public PrintableLayerPart
{
public:
    Polygons& getOutline();
    AABB getBoundaryBox();
    int64_t getZ();
    PrintableMeshLayerPart(SliceDataStorage& storage, unsigned int mesh_idx, unsigned int layer_idx, unsigned int part_idx);

protected:
    void writeGcode(GCodePlanner& gcode_layer);
private:
    const unsigned int mesh_idx;
    const unsigned int layer_idx;
    const unsigned int part_idx;
    SliceMeshStorage& mesh;
    SliceLayer& layer;
    SliceLayerPart& part;


    /*!
     * Generate the insets for the walls of a given layer part.
     * \param gcodeLayer The initial planning of the gcode of the layer.
     * \param z_seam_type dir3ective for where to start the outer paerimeter of a part
     */
    void processInsets(GCodePlanner& gcodeLayer, EZSeamType z_seam_type);

    /*!
     * Add thicker (multiple layers) sparse infill for a given part in a layer.
     * \param gcodeLayer The initial planning of the gcode of the layer.
     * \param infill_line_distance The distance between the infill lines
     * \param infill_overlap The fraction of the extrusion width by which the infill overlaps with the wall insets.
     * \param fillAngle The angle in the XY plane at which the infill is generated.
     * \param extrusionWidth extrusionWidth
     */
    void processMultiLayerInfill(GCodePlanner& gcodeLayer, int infill_line_distance, double infill_overlap, int fillAngle, int extrusionWidth); 

    /*!
     * Add normal sparse infill for a given part in a layer.
     * \param gcodeLayer The initial planning of the gcode of the layer.
     * \param infill_line_distance The distance between the infill lines
     * \param infill_overlap The fraction of the extrusion width by which the infill overlaps with the wall insets.
     * \param fillAngle The angle in the XY plane at which the infill is generated.
     * \param extrusionWidth extrusionWidth
     */
    void processSingleLayerInfill(GCodePlanner& gcodeLayer, int infill_line_distance, double infill_overlap, int fillAngle, int extrusionWidth);

    /*!
     * Add the gcode of the top/bottom skin of the given part.
     * \param gcodeLayer The initial planning of the gcode of the layer.
     * \param mesh The mesh for which to add to the gcode.
     * \param part The part for which to create gcode
     * \param layer_nr The current layer number.
     * \param infill_overlap The fraction of the extrusion width by which the infill overlaps with the wall insets.
     * \param fillAngle The angle in the XY plane at which the infill is generated.
     * \param extrusionWidth extrusionWidth
     */
    void processSkin(GCodePlanner& gcode_layer, double infill_overlap, int infill_angle, int extrusion_width);
    
};

} // namespace cura

#endif // PRINTABLE_MESH_LAYER_PART_H