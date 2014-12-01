/** Copyright (C) 2013 David Braam - Released under terms of the AGPLv3 License */
#ifndef POLYGON_OPTIMIZER_H
#define POLYGON_OPTIMIZER_H

#include "utils/polygon.h"

namespace cura {

extern int polygonL1Resolution;
extern int polygonL2Resolution;

void optimizePolygon(PolygonRef poly);

void optimizePolygons(Polygons& polys);

void setPolygonsResolution(int polygonL1Resolution, int polygonL2Resolution);

}//namespace cura

#endif//POLYGON_OPTIMIZER_H
