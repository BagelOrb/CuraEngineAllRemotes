/** Copyright (C) 2013 David Braam - Released under terms of the AGPLv3 License */
#ifndef BRIDGE_H
#define BRIDGE_H

#include "sliceDataStorage.h"
typedef signed long long cInt;

namespace cura {

int bridgeAngle(Polygons outline, SliceLayer* prevLayer);
cInt measureAngleCandidate( Polygons outline,  Polygons prevLayerPolygons);


}//namespace cura

#endif//BRIDGE_H
