/** Copyright (C) 2013 David Braam - Released under terms of the AGPLv3 License */
#include "skin.h"

#include "utils/remove_utils.h"

#include <algorithm>
namespace cura {

Polygons& getSkinOutline(SliceLayerPart* part)
{
    return part->skinOutline;
}

Polygons& get5050Outline(SliceLayerPart* part)
{
    return part->up5050Outline;
}

void generateSkins(int layerNr, SliceVolumeStorage& storage, int extrusionWidth, int downSkinCount, int upSkinCount, int infillOverlap, bool skin)
{
    SliceLayer* layer = &storage.layers[layerNr];

    for(SliceLayerPart& part : layer->parts)
    {
        Polygons upskin = part.insets[part.insets.size() - 1].offset(-extrusionWidth/2);
        Polygons downskin = upskin;
        
        if (part.insets.size() > 1)
        {
            //Add thin wall filling by taking the area between the insets.
            Polygons thinWalls = part.insets[0].offset(-extrusionWidth / 2 - extrusionWidth * infillOverlap / 100).difference(part.insets[1].offset(extrusionWidth * 6 / 10));
            upskin.add(thinWalls);
            downskin.add(thinWalls);
        }
        if (static_cast<int>(layerNr - downSkinCount) >= 0)
        {
            SliceLayer* layer2 = &storage.layers[layerNr - downSkinCount];
            for(SliceLayerPart& comp : layer2->parts)
            {
                if (part.boundaryBox.hit(comp.boundaryBox))
                    downskin = downskin.difference(comp.insets[comp.insets.size() - 1]);
            }
        }
        if (static_cast<int>(layerNr + upSkinCount) < static_cast<int>(storage.layers.size()))
        {
            SliceLayer* layer2 = &storage.layers[layerNr + upSkinCount + up5050SkinCount];
            for(SliceLayerPart& comp : layer2->parts)
            {
                if (part.boundaryBox.hit(comp.boundaryBox))
                    upskin = upskin.difference(comp.insets[comp.insets.size() - 1]);
            }
        }
        
        Polygons& outline = skin ? getSkinOutline(part) : get5050Outline(part);

        outline = upskin.unionPolygons(downskin);

        double minAreaSize = (2 * M_PI * INT2MM(extrusionWidth) * INT2MM(extrusionWidth)) * 0.3;
        for(unsigned int i=0; i<outline.size(); i++)
        {
            double area = INT2MM(INT2MM(fabs(outline[i].area())));
            if (area < minAreaSize) // Only create an up/down skin if the area is large enough. So you do not create tiny blobs of "trying to fill"
            {
                outline.remove(i);
                i -= 1;
            }
        }
    }
}

void generateSparse(int layerNr, SliceVolumeStorage& storage, int extrusionWidth, int downSkinCount, int upSkinCount, int up5050SkinCount)
{
    SliceLayer* layer = &storage.layers[layerNr];

    for(SliceLayerPart& part : layer->parts)
    {
        Polygons sparse = part.insets[part.insets.size() - 1].offset(-extrusionWidth/2);
        Polygons downskin = sparse;
        Polygons upskin = sparse;
        
        if (static_cast<int>(layerNr - downSkinCount) >= 0)
        {
            SliceLayer* layer2 = &storage.layers[layerNr - downSkinCount];
            for(SliceLayerPart& comp : layer2->parts)
            {
                if (part.boundaryBox.hit(comp.boundaryBox))
                {
                    if (comp.insets.size() > 1)
                    {
                        downskin = downskin.difference(comp.insets[comp.insets.size() - 2]);
                    }else{
                        downskin = downskin.difference(comp.insets[comp.insets.size() - 1]);
                    }
                }
            }
        }
        if (static_cast<int>(layerNr + upSkinCount) < static_cast<int>(storage.layers.size()))
        {
            SliceLayer* layer2 = &storage.layers[layerNr + upSkinCount + up5050SkinCount];
            for(SliceLayerPart& comp : layer2->parts)
            {
                if (part.boundaryBox.hit(comp.boundaryBox))
                {
                    if (comp.insets.size() > 1)
                    {
                        upskin = upskin.difference(comp.insets[comp.insets.size() - 2]);
                    }else{
                        upskin = upskin.difference(comp.insets[comp.insets.size() - 1]);
                    }
                }
            }
        }
        
        Polygons result = upskin.unionPolygons(downskin);

        double minAreaSize = 3.0;//(2 * M_PI * INT2MM(config.extrusionWidth) * INT2MM(config.extrusionWidth)) * 3;
        remove_if(result, removePolygon(minAreaSize));

        part.sparseOutline = sparse.difference(result);
    }
}

}//namespace cura
