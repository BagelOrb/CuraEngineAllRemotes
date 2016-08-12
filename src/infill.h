/** Copyright (C) 2013 David Braam - Released under terms of the AGPLv3 License */
#ifndef INFILL_H
#define INFILL_H

#include "utils/polygon.h"
#include "utils/logoutput.h"


namespace cura {


// CUSTOM FILLS
void generateChamberedInfill(Polygons outline, Polygons& result, int extrusionWidth, int inset_value, int layerNr);
void generateDoubleConcentricInfill(Polygons outline, Polygons& result, int extrusionWidth, int inset_value);
void generateAxisChamberedInfill(Polygons outline, Polygons& result, int extrusionWidth, int inset_value, int layerNr);
// CURA STANDARD FILLS
void generateConcentricInfill(Polygons outline, Polygons& result, int inset_value);
void generateAutomaticInfill(const Polygons& in_outline, Polygons& result, int extrusionWidth, int lineSpacing, int infillOverlap, double rotation);
void generateGridInfill(const Polygons& in_outline, Polygons& result, int extrusionWidth, int lineSpacing, int infillOverlap, double rotation);
void generateLineInfill(const Polygons& in_outline, Polygons& result, int extrusionWidth, int lineSpacing, int infillOverlap, double rotation);

}//namespace cura

#endif//INFILL_H
