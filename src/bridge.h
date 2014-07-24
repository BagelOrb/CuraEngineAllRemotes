/** Copyright (C) 2013 David Braam - Released under terms of the AGPLv3 License */
#ifndef BRIDGE_H
#define BRIDGE_H

#include "sliceDataStorage.h"
typedef signed long long cInt;

namespace cura {

cInt measureAngleCandidate(cura::Polygons outline,  cura::Polygons prevLayerPolygons, double angle);

int bridgeAngle(Polygons outline, SliceLayer* prevLayer);



}//namespace cura

#endif//BRIDGE_H
