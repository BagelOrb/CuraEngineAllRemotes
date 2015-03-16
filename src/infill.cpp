/** Copyright (C) 2013 David Braam - Released under terms of the AGPLv3 License */
#include "infill.h"

#include <iostream>
#include <fstream>

#include <opencv2/imgproc/imgproc.hpp>

namespace cura {

void generateConcentricInfill(Polygons outline, Polygons& result, int inset_value)
{
    while(outline.size() > 0)
    {
        for (unsigned int polyNr = 0; polyNr < outline.size(); polyNr++)
        {
            PolygonRef r = outline[polyNr];
            result.add(r);
        }
        outline = outline.offset(-inset_value);
    }
}

void generateAutomaticInfill(const Polygons& in_outline, Polygons& result,
                             int extrusionWidth, int lineSpacing,
                             int infillOverlap, double rotation)
{
    if (lineSpacing > extrusionWidth * 4)
    {
        generateGridInfill(in_outline, result, extrusionWidth, lineSpacing,
                           infillOverlap, rotation);
    }
    else
    {
        generateLineInfill(in_outline, result, extrusionWidth, lineSpacing,
                           infillOverlap, rotation);
    }
}

void generateGridInfill(const Polygons& in_outline, Polygons& result,
                        int extrusionWidth, int lineSpacing, int infillOverlap,
                        double rotation)
{
    generateLineInfill(in_outline, result, extrusionWidth, lineSpacing * 2,
                       infillOverlap, rotation);
    generateLineInfill(in_outline, result, extrusionWidth, lineSpacing * 2,
                       infillOverlap, rotation + 90);
}

int compare_int64_t(const void* a, const void* b)
{
    int64_t n = (*(int64_t*)a) - (*(int64_t*)b);
    if (n < 0) return -1;
    if (n > 0) return 1;
    return 0;
}

void generateLineInfill(const Polygons& in_outline, Polygons& result, int extrusionWidth, int lineSpacing, int infillOverlap, double rotation)
{
    Polygons outline = in_outline.offset(extrusionWidth * infillOverlap / 100);
    PointMatrix matrix(rotation);
    
    outline.applyMatrix(matrix);
    
    AABB boundary(outline);
    
    boundary.min.X = ((boundary.min.X / lineSpacing) - 1) * lineSpacing;
    int lineCount = (boundary.max.X - boundary.min.X + (lineSpacing - 1)) / lineSpacing;
    vector<vector<int64_t> > cutList;
    for(int n=0; n<lineCount; n++)
        cutList.push_back(vector<int64_t>());

    for(unsigned int polyNr=0; polyNr < outline.size(); polyNr++)
    {
        Point p1 = outline[polyNr][outline[polyNr].size()-1];
        for(unsigned int i=0; i < outline[polyNr].size(); i++)
        {
            Point p0 = outline[polyNr][i];
            int idx0 = (p0.X - boundary.min.X) / lineSpacing;
            int idx1 = (p1.X - boundary.min.X) / lineSpacing;
            int64_t xMin = p0.X, xMax = p1.X;
            if (p0.X > p1.X) { xMin = p1.X; xMax = p0.X; }
            if (idx0 > idx1) { int tmp = idx0; idx0 = idx1; idx1 = tmp; }
            for(int idx = idx0; idx<=idx1; idx++)
            {
                int x = (idx * lineSpacing) + boundary.min.X + lineSpacing / 2;
                if (x < xMin) continue;
                if (x >= xMax) continue;
                int y = p0.Y + (p1.Y - p0.Y) * (x - p0.X) / (p1.X - p0.X);
                cutList[idx].push_back(y);
            }
            p1 = p0;
        }
    }
    
    int idx = 0;
    for(int64_t x = boundary.min.X + lineSpacing / 2; x < boundary.max.X; x += lineSpacing)
    {
        qsort(cutList[idx].data(), cutList[idx].size(), sizeof(int64_t), compare_int64_t);
        for(unsigned int i = 0; i + 1 < cutList[idx].size(); i+=2)
        {
            if (cutList[idx][i+1] - cutList[idx][i] < extrusionWidth / 5)
                continue;
            PolygonRef p = result.newPoly();
            p.add(matrix.unapply(Point(x, cutList[idx][i])));
            p.add(matrix.unapply(Point(x, cutList[idx][i+1])));
        }
        idx += 1;
    }
}


void generateVoronoiInfill2(const Polygons& in_outline, Polygons& result,
                           int extrusionWidth, int lineSpacing, int infillOverlap,
                           std::vector<Point3> inputPoints)
{
    Polygons outline = in_outline.offset(extrusionWidth * infillOverlap / 100);
    AABB boundary(outline);
    
    boundary.min.X = ((boundary.min.X / lineSpacing) - 1) * lineSpacing;
    int lineCount = (boundary.max.X - boundary.min.X + (lineSpacing - 1)) / lineSpacing;

    std::cerr << "bounds: " << boundary.min << boundary.max << "\n";

#if 0
    // TEST: just make a line crossing between boundary corners
    PolygonRef p = result.newPoly();
    p.add(Point(boundary.min.X, boundary.min.Y));
    p.add(Point(boundary.max.X, boundary.max.Y));

#else

    std::cerr << "number of points " << inputPoints.size() << "\n";

    // FIXME: code to do mesh intersect? OR let Cura handle it
    // Calculate Voronoi

    cv::Rect rect(boundary.min.X, boundary.min.Y,
                  boundary.max.X-boundary.min.X, boundary.max.Y-boundary.min.Y);

    std::cerr << "rect " << rect << "\n";

    cv::Subdiv2D subdiv(rect);
    for( int i = 0; i < inputPoints.size(); i++ )
    {
        const Point3 ip = inputPoints.at(i);
        const bool isInside = outline.inside(Point(ip.x, ip.y));
        // std::cerr << "p " << ip.x << ", " << ip.y << " inside? " << isInside << "\n";

        // Clip to bounds
        if (!isInside) {
            continue;
        }

        cv::Point2f fp(ip.x, ip.y);
        // TODO: apply same transformation as cura did to original object, in case of scale/rot/translate
        try {
            subdiv.insert(fp);
        } catch (const std::exception &e) {
            // TEMP: ignored
        }
    }

    std::vector<std::vector<cv::Point2f> > facets;
    std::vector<cv::Point2f> centers;
    subdiv.getVoronoiFacetList(std::vector<int>(), facets, centers);

    std::cerr << "number of facets " << facets.size() << "\n";

    // Add the voronoi lines as infill
    for( size_t i = 0; i < facets.size(); i++ )
    {
        PolygonRef p = result.newPoly();
        for( size_t j = 0; j < facets[i].size(); j++ ) {
            cv::Point2f ip = facets[i][j];

            const bool isInside = outline.inside(Point(ip.x, ip.y));
            // std::cerr << "p " << ip.x << ", " << ip.y << " inside? " << isInside << "\n";

            // Clip to bounds
            if (!isInside) {
                continue;
            }
            p.add(Point(ip.x, ip.y));
        }
    }
#endif
}


}//namespace cura
