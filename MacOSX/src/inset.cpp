/** Copyright (C) 2013 David Braam - Released under terms of the AGPLv3 License */
#include "inset.h"
#include "polygonOptimizer.h"

#include "utils/remove_utils.h"

#include <algorithm>
namespace cura {

void generateInsets(SliceLayerPart* part, int offset, int bulgeOffset, int insetCount)
{
    part->combBoundery = part->outline.offset(-offset-bulgeOffset);
    if (insetCount == 0)
    {
        part->insets.push_back(part->outline);
        return;
    }
    
    for(int i=0; i<insetCount; i++)
    {
        part->insets.push_back(Polygons());
        part->insets[i] = part->outline.offset(-offset * i - offset/2 - bulgeOffset);
        optimizePolygons(part->insets[i]);
        if (part->insets[i].size() < 1)
        {
            part->insets.pop_back();
            break;
        }
    }
}

void generateInsets(SliceLayer* layer, int offset, int bulgeOffset, int insetCount)
{
    for(SliceLayerPart& part : layer->parts)
    {
        generateInsets(&layer->parts[partNr], offset, bulgeOffset, insetCount);
    }
    
    //Remove the parts which did not generate an inset. As these parts are too small to print,
    // and later code can now assume that there is always minimal 1 inset line.
    remove_if(layer->parts, remove_part);
}

}//namespace cura
