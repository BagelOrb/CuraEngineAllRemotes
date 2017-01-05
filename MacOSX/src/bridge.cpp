/** Copyright (C) 2013 David Braam - Released under terms of the AGPLv3 License */
#include "bridge.h"
#include <algorithm>



namespace cura {



cInt measureAngleCandidate(cura::Polygons outline, cura::Polygons prevLayerPolygons, double angle)
{
    std::vector<cInt> crossings;

    PointMatrix matrix(angle);
    outline.applyMatrix(matrix);
    outline = outline.offset(1000);
    prevLayerPolygons.applyMatrix(matrix);

    cInt totalLengthSquare=0;

    cura::AABB boundaryBox(outline);

//    Polygons prevLayerPolygons;
//    for(auto prevLayerPart : prevLayer->parts)
//    {
//        prevLayerPolygons.add(prevLayerPart.outline);
//    }

    cInt minX = boundaryBox.min.X;
    cInt maxX = boundaryBox.max.X;
    int penaltyCount = 0;
    int noPenaltyCount = 0;

    for(int x=minX; x<maxX; x+=1000)
    {
        for(unsigned int polyNr=0; polyNr<outline.size(); ++polyNr)
        {
            for(unsigned int startPoint=0; startPoint < outline[polyNr].size()-1; startPoint++)
            {
                unsigned int endPoint;
                if(startPoint==outline[polyNr].size())
                    endPoint = 0;
                else
                    endPoint=startPoint+1;

                cInt startX = outline[polyNr][startPoint].X;
                cInt startY = outline[polyNr][startPoint].Y;
                cInt endX = outline[polyNr][endPoint].X;
                cInt endY = outline[polyNr][endPoint].Y;
                if((startX>x && endX<x) || (startX<x && endX>x))
                {
                    //cInt y = (x-startX) / (endX-startX * (endY-startY) + startY);
                    cInt y = (x-startX) * (endY-startY)/(endX-startX) + startY;
                    crossings.push_back(y);
                }
                std::sort(crossings.begin(), crossings.end());

                if(crossings.size()>1)
                {
                    for(unsigned int i=0;i<crossings.size()-1;i+=2)
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
                            penaltyCount++;
                            //pathLength+=penalty;
                        }
                        else
                        {
                            noPenaltyCount++;
                        }

                        totalLengthSquare+=pathLength^2;
                    }
                }
            }
            crossings.clear();
        }
    }

    if(noPenaltyCount == 0)
    {
        totalLengthSquare = totalLengthSquare*penaltyCount*10000;
    }
    else
    {
        totalLengthSquare = totalLengthSquare * ((penaltyCount/noPenaltyCount)+1);
    }

    return totalLengthSquare;
}


int bridgeAngle(Polygons outline, SliceLayer* prevLayer)
{
    Polygons prevLayerPolygons;
    for(int i=0; i<prevLayer->parts.size();++i)
    {
        prevLayerPolygons.add(prevLayer->parts[i].outline);
    }
    

    cInt bestMeasure = 999999999;
    int bestAngle = 0;
    for(int angle = 0; angle<180;++angle)
    {
        cInt measure = measureAngleCandidate(outline, prevLayerPolygons, angle);

        if(measure < bestMeasure)
        {
            bestMeasure = measure;
            bestAngle = angle;
        }
    }
    
    return bestAngle;
}

    
//    AABB boundaryBox(outline);
//    //To detect if we have a bridge, first calculate the intersection of the current layer with the previous layer.
//    // This gives us the islands that the layer rests on.
//    Polygons islands;
//    for(auto prevLayerPart : prevLayer->parts)
//    {
//        if (!boundaryBox.hit(prevLayerPart.boundaryBox))
//            continue;
//
//        islands.add(outline.intersection(prevLayerPart.outline));
//    }
//    if (islands.size() > 5 || islands.size() < 1)
//        return -1;
//
//    //Next find the 2 largest islands that we rest on.
//    double area1 = 0;
//    double area2 = 0;
//    int idx1 = -1;
//    int idx2 = -1;
//    for(unsigned int n=0; n<islands.size(); n++)
//    {
//        //Skip internal holes
//        if (!islands[n].orientation())
//            continue;
//        double area = fabs(islands[n].area());
//        if (area > area1)
//        {
//            if (area1 > area2)
//            {
//                area2 = area1;
//                idx2 = idx1;
//            }
//            area1 = area;
//            idx1 = n;
//        }else if (area > area2)
//        {
//            area2 = area;
//            idx2 = n;
//        }
//    }
//
//    if (idx1 < 0 || idx2 < 0)
//        return -1;
//
//    Point center1 = islands[idx1].centerOfMass();
//    Point center2 = islands[idx2].centerOfMass();
//
//    return angle(center2 - center1);
//}

//int measureBridgingQuality(PolygonRef outline, SliceLayer* prevLayer)

}//namespace cura

