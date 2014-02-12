/** Copyright (C) 2013 David Braam - Released under terms of the AGPLv3 License */
#ifndef INSET_H
#define INSET_H

#include "sliceDataStorage.h"

void generateInsets(SliceLayerPart* part, int offset, int insetCount, int perimInset);

void generateInsets(SliceLayer* layer, int offset, int insetCount, int perimInset);

#endif//INSET_H
