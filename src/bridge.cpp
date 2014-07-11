/** Copyright (C) 2013 David Braam - Released under terms of the AGPLv3 License */
#include "bridge.h"
#include <algorithm>



namespace cura {

std::vector<cInt> crossings;

cInt measureAngleCandidate(Polygons outline,  Polygons prevLayerPolygons)
{
    const int penalty = 100*1000; // 100mm
    cInt totalLengthSquare=0;

    AABB boundaryBox(outline);

//    Polygons prevLayerPolygons;
//    for(auto prevLayerPart : prevLayer->parts)
//    {
//        prevLayerPolygons.add(prevLayerPart.outline);
//    }

    cInt minX = boundaryBox.min.X;
    cInt maxX = boundaryBox.max.X;

    for(int x=minX; x<maxX; x+=1000)
    {
        for(unsigned int pointNr=0; pointNr < outline[0].size()-1; ++pointNr)
        {
            if(outline[0][pointNr].X<x && outline[0][pointNr+1].X>x)
            {
                cInt y = (x-outline[0][pointNr].X) / (outline[0][pointNr+1].X-outline[0][pointNr].X * (outline[0][pointNr+1].Y-outline[0][pointNr].Y) + outline[0][pointNr].Y);
                crossings.push_back(y);
            }
            else if(outline[0][pointNr].X>x && outline[0][pointNr+1].X<x)
            {
                cInt y = (x-outline[0][pointNr].X) / (outline[0][pointNr+1].X-outline[0][pointNr].X * (outline[0][pointNr+1].Y-outline[0][pointNr].Y) + outline[0][pointNr].Y);
                crossings.push_back(y);
            }
            std::sort(crossings.begin(), crossings.end());

            for(unsigned int i=0;i<crossings.size()-1;++i)
            {
                Point p1,p2;
                p1.X=x;
                p1.Y=crossings[i];
                p2.X=x;
                p2.Y=crossings[i+1];

                cInt pathLength = p2.Y-p1.Y;

                if(!prevLayerPolygons.inside(p1) || !prevLayerPolygons.inside(p2))
                {
                    // one endpoint doesn't lie on the previous layer. This path gets a penalty!
                    pathLength+=penalty;
                }

                totalLengthSquare+=pathLength^2;
            }
        }
    }

    return totalLengthSquare;
}




int bridgeAngle(Polygons outline, SliceLayer* prevLayer)
{
    AABB boundaryBox(outline);
    //To detect if we have a bridge, first calculate the intersection of the current layer with the previous layer.
    // This gives us the islands that the layer rests on.
    Polygons islands;
    for(auto prevLayerPart : prevLayer->parts)
    {
        if (!boundaryBox.hit(prevLayerPart.boundaryBox))
            continue;
        
        islands.add(outline.intersection(prevLayerPart.outline));
    }
    if (islands.size() > 5 || islands.size() < 1)
        return -1;
    
    //Next find the 2 largest islands that we rest on.
    double area1 = 0;
    double area2 = 0;
    int idx1 = -1;
    int idx2 = -1;
    for(unsigned int n=0; n<islands.size(); n++)
    {
        //Skip internal holes
        if (!islands[n].orientation())
            continue;
        double area = fabs(islands[n].area());
        if (area > area1)
        {
            if (area1 > area2)
            {
                area2 = area1;
                idx2 = idx1;
            }
            area1 = area;
            idx1 = n;
        }else if (area > area2)
        {
            area2 = area;
            idx2 = n;
        }
    }
    
    if (idx1 < 0 || idx2 < 0)
        return -1;
    
    Point center1 = islands[idx1].centerOfMass();
    Point center2 = islands[idx2].centerOfMass();

    return angle(center2 - center1);
}

//int measureBridgingQuality(PolygonRef outline, SliceLayer* prevLayer)
//{
//    // find Xmin and Xmax
//    int xMin = 0xFFFFFFFF;
//    int xMax = 0;
//    for(int i=0; i<outline.size(); ++i)
//    {
//        int x = outline[i].X;
//        if(x < xMin)
//            xMin = x;
//        if(x > xMax)
//            xMax = x;
//    }
//
//    for(int x=xMin;x<xMax;x+=1000)
//    {
//        vector<int> intersections;
//        for(int polyPoint=0; polyPoint<outline.size()-1; ++polyPoint)
//        {
//            if(outline[polyPoint]<x && outline[polyPoint+1]>x)
//            {
//                // We have an "incoming" intersection
//                intersections.push_back(y);
//            }
//            else if(outline[polyPoint]>x && outline[polyPoint+1]<x)
//            {
//                //we have an "outgoing intersecion
//
//            }
//        }
//    }
//
//}

}//namespace cura

