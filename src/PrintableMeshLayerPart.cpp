#include "PrintableMeshLayerPart.h"
#include "PrintableLayerPart.h"
#include "infill.h"
#include "bridge.h"
#include "settings.h"

namespace cura
{

PrintableMeshLayerPart::PrintableMeshLayerPart(SliceDataStorage& storage, unsigned int mesh_idx, unsigned int layer_idx, unsigned int part_idx)
: PrintableLayerPart(storage, (unsigned int) storage.meshes[mesh_idx].getSettingAsCount("extruder_nr"), layer_idx)
, mesh_idx(mesh_idx)
, layer_idx(layer_idx)
, part_idx(part_idx)
, mesh(storage.meshes[mesh_idx])
, layer(mesh.layers[layer_idx])
, part(layer.parts[layer_idx])
{
}

    
Polygons& PrintableMeshLayerPart::getOutline()
{
    return part.outline;
};

AABB PrintableMeshLayerPart::getBoundaryBox()
{
    return part.boundaryBox;
}

int64_t PrintableMeshLayerPart::getZ()
{
    return layer.printZ;
}

void PrintableMeshLayerPart::writeGcode(GCodePlanner& gcode_layer)
{
    if (int(layer_idx) > mesh.layer_nr_max_filled_layer)
    {
        return;
    }

    if (layer.parts.size() == 0)
    {
        return;
    }
    
//     setExtruder_addPrime(storage, gcode_layer, layer_idx, mesh.getSettingAsIndex("extruder_nr"));
// TODO

    
    EZSeamType z_seam_type = mesh.getSettingAsZSeamType("z_seam_type");
    
    bool skin_alternate_rotation = mesh.getSettingBoolean("skin_alternate_rotation") && ( mesh.getSettingAsCount("top_layers") >= 4 || mesh.getSettingAsCount("bottom_layers") >= 4 );


    EFillMethod infill_pattern = mesh.getSettingAsFillMethod("infill_pattern");
    int infill_angle = 45;
    if ((infill_pattern==EFillMethod::LINES || infill_pattern==EFillMethod::ZIG_ZAG) && layer_idx & 1)
    {
        infill_angle += 90;
    }
    int infill_line_width =  mesh.infill_config[0].getLineWidth();
    
    int infill_line_distance = mesh.getSettingInMicrons("infill_line_distance");
    double infill_overlap = mesh.getSettingInPercentage("infill_overlap");
    
    if (mesh.getSettingBoolean("infill_before_walls"))
    {
        processMultiLayerInfill(gcode_layer, infill_line_distance, infill_overlap, infill_angle, infill_line_width);
        processSingleLayerInfill(gcode_layer, infill_line_distance, infill_overlap, infill_angle, infill_line_width);
    }
    
    processInsets(gcode_layer, z_seam_type);

    if (!mesh.getSettingBoolean("infill_before_walls"))
    {
        processMultiLayerInfill(gcode_layer, infill_line_distance, infill_overlap, infill_angle, infill_line_width);
        processSingleLayerInfill(gcode_layer, infill_line_distance, infill_overlap, infill_angle, infill_line_width);
    }

    EFillMethod skin_pattern = mesh.getSettingAsFillMethod("top_bottom_pattern");
    int skin_angle = 45;
    if ((skin_pattern == EFillMethod::LINES || skin_pattern == EFillMethod::ZIG_ZAG) && layer_idx & 1)
    {
        skin_angle += 90; // should coincide with infill_angle (if both skin and infill are lines) so that the first top layer is orthogonal to the last infill layer
    }
    if (skin_alternate_rotation && ( layer_idx / 2 ) & 1)
    {
        skin_angle -= 45;
    }
    
    int64_t skin_overlap = infill_overlap;
    processSkin(gcode_layer, skin_overlap, skin_angle, mesh.skin_config.getLineWidth());    
    
    //After a layer part, make sure the nozzle is inside the comb boundary, so we do not retract on the perimeter.
    if (!mesh.getSettingBoolean("magic_spiralize") || static_cast<int>(layer_idx) < mesh.getSettingAsCount("bottom_layers"))
    {
        gcode_layer.moveInsideCombBoundary(mesh.getSettingInMicrons("machine_nozzle_size") * 1);
    }
}


void PrintableMeshLayerPart::processInsets(GCodePlanner& gcode_layer, EZSeamType z_seam_type)
{
    bool compensate_overlap = mesh.getSettingBoolean("travel_compensate_overlapping_walls_enabled");
    if (mesh.getSettingAsCount("wall_line_count") > 0)
    {
        if (mesh.getSettingBoolean("magic_spiralize"))
        {
            if (static_cast<int>(layer_idx) >= mesh.getSettingAsCount("bottom_layers"))
            {
                mesh.inset0_config.spiralize = true;
            }
            if (static_cast<int>(layer_idx) == mesh.getSettingAsCount("bottom_layers") && part.insets.size() > 0)
            {
                gcode_layer.addPolygonsByOptimizer(part.insets[0], &mesh.insetX_config);
            }
        }
        for (int inset_number = part.insets.size() - 1; inset_number > -1; inset_number--)
        {
            if (inset_number == 0)
            {
                if (!compensate_overlap)
                {
                    gcode_layer.addPolygonsByOptimizer(part.insets[0], &mesh.inset0_config, nullptr, z_seam_type);
                }
                else
                {
                    Polygons& outer_wall = part.insets[0];
                    WallOverlapComputation wall_overlap_computation(outer_wall, mesh.getSettingInMicrons("wall_line_width_0"));
                    gcode_layer.addPolygonsByOptimizer(outer_wall, &mesh.inset0_config, &wall_overlap_computation, z_seam_type);
                }
            }
            else
            {
                gcode_layer.addPolygonsByOptimizer(part.insets[inset_number], &mesh.insetX_config);
            }
        }
    }
}

void PrintableMeshLayerPart::processMultiLayerInfill(GCodePlanner& gcode_layer, int infill_line_distance, double infill_overlap, int infill_angle, int extrusion_width)
{
    if (infill_line_distance > 0)
    {
        //Print the thicker infill lines first. (double or more layer thickness, infill combined with previous layers)
        for (unsigned int n = 1; n < part.infill_area.size(); n++)
        {
            EFillMethod infill_pattern = mesh.getSettingAsFillMethod("infill_pattern");
            Infill infill_comp(infill_pattern, part.infill_area[n], 0, false, extrusion_width, infill_line_distance, infill_overlap, infill_angle, false, false);
            Polygons infill_polygons;
            Polygons infill_lines;
            infill_comp.generate(infill_polygons, infill_lines, nullptr);
            gcode_layer.addPolygonsByOptimizer(infill_polygons, &mesh.infill_config[n]);
            gcode_layer.addLinesByOptimizer(infill_lines, &mesh.infill_config[n], (infill_pattern == EFillMethod::ZIG_ZAG)? SpaceFillType::PolyLines : SpaceFillType::Lines);
        }
    }
}

void PrintableMeshLayerPart::processSingleLayerInfill(GCodePlanner& gcode_layer, int infill_line_distance, double infill_overlap, int infill_angle, int extrusion_width)
{
    
    if (infill_line_distance == 0 || part.infill_area.size() == 0)
    {
        return;
    }
        
    //Combine the 1 layer thick infill with the top/bottom skin and print that as one thing.
    Polygons infill_polygons;
    Polygons infill_lines;
    
    EFillMethod pattern = mesh.getSettingAsFillMethod("infill_pattern");
    Infill infill_comp(pattern, part.infill_area[0], 0, false, extrusion_width, infill_line_distance, infill_overlap, infill_angle, false, false);
    infill_comp.generate(infill_polygons, infill_lines, nullptr);
    gcode_layer.addPolygonsByOptimizer(infill_polygons, &mesh.infill_config[0]);
    if (pattern == EFillMethod::GRID || pattern == EFillMethod::LINES || pattern == EFillMethod::TRIANGLES)
    {
        gcode_layer.addLinesByOptimizer(infill_lines, &mesh.infill_config[0], SpaceFillType::Lines, mesh.getSettingInMicrons("infill_wipe_dist")); 
    }
    else 
    {
        gcode_layer.addLinesByOptimizer(infill_lines, &mesh.infill_config[0], (pattern == EFillMethod::ZIG_ZAG)? SpaceFillType::PolyLines : SpaceFillType::Lines); 
    }
}


void PrintableMeshLayerPart::processSkin(GCodePlanner& gcode_layer, double infill_overlap, int infill_angle, int extrusion_width)
{
    for(SkinPart& skin_part : part.skin_parts) // TODO: optimize parts order
    {
        Polygons skin_polygons;
        Polygons skin_lines;
        
        EFillMethod pattern = mesh.getSettingAsFillMethod("top_bottom_pattern");
        int bridge = -1;
        if (layer_idx > 0)
        {
            bridge = bridgeAngle(skin_part.outline, &mesh.layers[layer_idx-1]);
        }
        if (bridge > -1)
        {
            pattern = EFillMethod::LINES;
        } 
        Polygons* inner_skin_outline = nullptr;
        int offset_from_inner_skin_outline = 0;
        if (pattern != EFillMethod::CONCENTRIC)
        {
            for (Polygons& skin_perimeter : skin_part.insets)
            {
                gcode_layer.addPolygonsByOptimizer(skin_perimeter, &mesh.skin_config); // add polygons to gcode in inward order
            }
            if (skin_part.insets.size() > 0)
            {
                inner_skin_outline = &skin_part.insets.back();
                offset_from_inner_skin_outline = -extrusion_width/2;
                if (mesh.getSettingAsFillPerimeterGapMode("fill_perimeter_gaps") != FillPerimeterGapMode::NOWHERE)
                {
                    Polygons result_polygons; // should remain empty, since we're only allowing for lines infill
                    Polygons* in_between = nullptr;
                    bool avoidOverlappingPerimeters = false;
                    int line_distance = extrusion_width;
                    int outline_offset = 0;
                    Infill infill_comp(EFillMethod::LINES, skin_part.perimeterGaps, outline_offset, avoidOverlappingPerimeters, extrusion_width, line_distance, infill_overlap, infill_angle);
                    infill_comp.generate(result_polygons, skin_lines, in_between);
                }
            } 
        }
        
        if (inner_skin_outline == nullptr)
        {
            inner_skin_outline = &skin_part.outline;
        }
        
        Infill infill_comp(pattern, *inner_skin_outline, offset_from_inner_skin_outline, mesh.getSettingBoolean("remove_overlapping_walls_x_enabled"), extrusion_width, extrusion_width, infill_overlap, infill_angle, false, false);
        infill_comp.generate(skin_polygons, skin_lines, &part.perimeterGaps);
        
        gcode_layer.addPolygonsByOptimizer(skin_polygons, &mesh.skin_config);
        
        if (pattern == EFillMethod::GRID || pattern == EFillMethod::LINES || pattern == EFillMethod::TRIANGLES)
        {
            gcode_layer.addLinesByOptimizer(skin_lines, &mesh.skin_config, SpaceFillType::Lines, mesh.getSettingInMicrons("infill_wipe_dist")); 
        }
        else 
        {
            gcode_layer.addLinesByOptimizer(skin_lines, &mesh.skin_config, (pattern == EFillMethod::ZIG_ZAG)? SpaceFillType::PolyLines : SpaceFillType::Lines); 
        }
    }
    
    // handle gaps between perimeters etc.
    if (mesh.getSettingAsFillPerimeterGapMode("fill_perimeter_gaps") != FillPerimeterGapMode::NOWHERE)
    {
        Polygons perimeter_gap_lines;
        Polygons result_polygons; // should remain empty, since we're only allowing for lines infill
        Polygons* in_between = nullptr;
        bool avoidOverlappingPerimeters = false;
        int line_distance = extrusion_width;
        int outline_offset = 0;
        Infill infill_comp(EFillMethod::LINES, part.perimeterGaps, outline_offset, avoidOverlappingPerimeters, extrusion_width, line_distance, infill_overlap, infill_angle);
        infill_comp.generate(result_polygons, perimeter_gap_lines, in_between);
        
        gcode_layer.addLinesByOptimizer(perimeter_gap_lines, &mesh.skin_config, SpaceFillType::Lines, mesh.getSettingInMicrons("infill_wipe_dist"));
    }
}


} // namespace cura


