/** Copyright (C) 2013 David Braam - Released under terms of the AGPLv3 License */
#ifndef INFILL_H
#define INFILL_H

#include "utils/polygon.h"

#include <vector>

namespace cura {

void generateConcentricInfill(Polygons outline, Polygons& result, int inset_value);
void generateAutomaticInfill(const Polygons& in_outline, Polygons& result, int extrusionWidth, int lineSpacing, int infillOverlap, double rotation);
void generateGridInfill(const Polygons& in_outline, Polygons& result, int extrusionWidth, int lineSpacing, int infillOverlap, double rotation);
void generateLineInfill(const Polygons& in_outline, Polygons& result, int extrusionWidth, int lineSpacing, int infillOverlap, double rotation);
void generateVoronoiInfill2(const Polygons& in_outline, Polygons& result, int extrusionWidth, int lineSpacing, int infillOverlap,
                            std::vector<Point3> inputPoints);

}//namespace cura

#endif//INFILL_H
