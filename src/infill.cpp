/** Copyright (C) 2013 David Braam - Released under terms of the AGPLv3 License */
#include "infill.h"
// #include "rapidjson/include/reader.h"
namespace cura {

void generateAxisChamberedInfill(Polygons outline, Polygons& result, int extrusionWidth, int inset_value, int layerNr){
   
    int doubleGap = extrusionWidth * 1;

    double start_area = 0;
    double stop_area = 0;
    if(outline.size()  > 0){
       printf("generateChamberedInfill %f\n", outline[0].area());
        start_area = outline[0].area();
    }
    stop_area = start_area / 4.0;

    while(outline.size() > 0 && outline[0].area() > stop_area)
    {
        for (unsigned int polyNr = 0; polyNr < outline.size(); polyNr++)
        {
            PolygonRef r = outline[polyNr];
            result.add(r);
        }
        outline = outline.offset(-inset_value);
    }
    if(outline.size() > 0){
        for (unsigned int polyNr = 0; polyNr < outline.size(); polyNr++)
        {
            PolygonRef r = outline[polyNr];
            result.add(r);
        }
        outline = outline.offset(doubleGap);
    }
    if(outline.size() > 0){
        generateChamberedInfill(outline, result, extrusionWidth, inset_value * 2, layerNr);
    }
}
void generateChamberedInfill(Polygons outline, Polygons& result, int extrusionWidth, int inset_value, int layerNr){
    cura::logError("Layer %d\n", layerNr);
    int doubleGap = extrusionWidth * 1;
    
    while(outline.size() > 0)
    {   

        AABB boundary(outline); 

        double origin_x = boundary.min.X;
        double origin_y = boundary.min.Y;

        double height = boundary.max.Y - boundary.min.Y;
        double width = boundary.max.X - boundary.min.X;

        origin_x += width / 2.0;
        origin_y += height / 2.0;

        // printf("ORIGIN %f, %f \n", origin_x/1000, origin_y/1000);

        // double gap_factor = 5.0;
        // double x_gap = width / gap_factor / 2.0;
        // double y_gap = height / gap_factor / 2.0;

        double x_gap = 1750; // a fixed 1mm hole
        double y_gap = 1750; // a fixed 1mm hole
        double overeach = 200; 

        Polygons a_e; 
        Polygons b_e;
        ClipperLib::Path a; 
        ClipperLib::Path b; 
        ClipperLib::Path c; 


        a << ClipperLib::IntPoint(origin_x - x_gap, boundary.max.Y + overeach) << ClipperLib::IntPoint(origin_x + x_gap, boundary.max.Y + overeach) << 
            ClipperLib::IntPoint(origin_x + x_gap, boundary.min.Y - overeach) << ClipperLib::IntPoint(origin_x - x_gap, boundary.min.Y - overeach);
        
        b << ClipperLib::IntPoint(boundary.min.X - overeach, origin_y + y_gap) << ClipperLib::IntPoint(boundary.max.X + overeach, origin_y + y_gap) << 
            ClipperLib::IntPoint(boundary.max.X + overeach, origin_y - y_gap) << ClipperLib::IntPoint(boundary.min.X - overeach, origin_y - y_gap);

        a_e.add(a);
        b_e.add(b);
       
        Polygons holes_final = a_e.unionPolygons(b_e);
        
     
        Polygons walls;
        walls.add(outline);
        Polygons wall = walls.difference(outline.offset(-doubleGap));


        Polygons s = wall.difference(holes_final);
        // Polygons sp = outline.difference(s);
        for (unsigned int polyNr = 0; polyNr < s.size(); polyNr++)
        {
            PolygonRef r = s[polyNr];
            result.add(r);
        }        


        outline = outline.offset(-inset_value);
    }

}
// CUSTOM G-CODE - SINTARE PROJECT
void generateDoubleConcentricInfill(Polygons outline, Polygons& result, int extrusionWidth, int inset_value)
{
    int doubleGap = extrusionWidth * 3;
    
    while(outline.size() > 0)
    {
        // First Wall
        for (unsigned int polyNr = 0; polyNr < outline.size(); polyNr++)
        {
            PolygonRef r = outline[polyNr];
            result.add(r);
        }
        // Second Wall
        outline = outline.offset(-doubleGap);
        if(outline.size() <= 0) break;
        for (unsigned int polyNr = 0; polyNr < outline.size(); polyNr++)
        {
            PolygonRef r = outline[polyNr];
            result.add(r);
        }
        outline = outline.offset(-inset_value);
    }
}
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

}//namespace cura
