/** Copyright (C) 2013 David Braam - Released under terms of the AGPLv3 License */
#ifndef MODELFILE_H
#define MODELFILE_H
/**
modelFile contains the model loaders for the slicer. The model loader turns any format that it can read into a list of triangles with 3 X/Y/Z points.

The format returned is a Model class with an array of faces, which have integer points with a resolution of 1 micron. Giving a maximum object size of 4 meters.
**/

#include <vector>
#include "../utils/intpoint.h"
#include "../utils/floatpoint.h"

extern FILE* binaryMeshBlob;

#define SET_MIN(n, m) do { if ((m) < (n)) n = m; } while(0)
#define SET_MAX(n, m) do { if ((m) > (n)) n = m; } while(0)

/* A SimpleFace is a 3 dimensional model triangle with 3 points. These points are already converted to integers */
class SimpleFace
{
public:
    Point3 v[3];
    
    SimpleFace(Point3& v0, Point3& v1, Point3& v2) { v[0] = v0; v[1] = v1; v[2] = v2; }
};

/* A SimpleVolume is the most basic reprisentation of a 3D model. It contains all the faces as SimpleTriangles, with nothing fancy. */
class SimpleVolume
{
public:
    std::vector<SimpleFace> faces;
    
    void addFace(Point3& v0, Point3& v1, Point3& v2)
    {
        faces.push_back(SimpleFace(v0, v1, v2));
    }
    
    Point3 min()
    {
        if (faces.size() < 1)
            return Point3(0, 0, 0);
        Point3 ret = faces[0].v[0];
        for(SimpleFace& sf : faces)
        {
            for(int x=0; x<3; x++)
            {
                SET_MIN(ret.x, sf.v[x].x);
                SET_MIN(ret.y, sf.v[x].y);
                SET_MIN(ret.z, sf.v[x].z);
            }
        }
        return ret;
    }
    Point3 max()
    {
        if (faces.size() < 1)
            return Point3(0, 0, 0);
        Point3 ret = faces[0].v[0];
        for(SimpleFace& sf : faces)
        {
            for(int x=0; x<3; x++)
            {
                SET_MAX(ret.x, sf.v[x].x);
                SET_MAX(ret.y, sf.v[x].y);
                SET_MAX(ret.z, sf.v[x].z);
            }
        }
        return ret;
    }
};

//A SimpleModel is a 3D model with 1 or more 3D volumes.
class SimpleModel
{
public:
    std::vector<SimpleVolume> volumes;

    Point3 min()
    {
        if (volumes.size() < 1)
            return Point3(0, 0, 0);
        Point3 ret = volumes[0].min();
        for(SimpleVolume& sv : volumes)
        {
            Point3 v = sv.min();
            SET_MIN(ret.x, v.x);
            SET_MIN(ret.y, v.y);
            SET_MIN(ret.z, v.z);
        }
        return ret;
    }
    Point3 max()
    {
        if (volumes.size() < 1)
            return Point3(0, 0, 0);
        Point3 ret = volumes[0].max();
        for(SimpleVolume& sv : volumes)
        {
            Point3 v = sv.max();
            SET_MAX(ret.x, v.x);
            SET_MAX(ret.y, v.y);
            SET_MAX(ret.z, v.z);
        }
        return ret;
    }
};

SimpleModel* loadModelFromFile(const char* filename, FMatrix3x3& matrix);

#endif//MODELFILE_H
