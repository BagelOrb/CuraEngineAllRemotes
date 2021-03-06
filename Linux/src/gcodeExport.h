/** Copyright (C) 2013 David Braam - Released under terms of the AGPLv3 License */
#ifndef GCODEEXPORT_H
#define GCODEEXPORT_H

#include <stdio.h>

#include "settings.h"
#include "comb.h"
#include "utils/intpoint.h"
#include "utils/polygon.h"
#include "timeEstimate.h"

#define EN_FIRSTLINE 1	//Print some silk from the origin point to the starting point,then reset.

namespace cura {

//The GCodeExport class writes the actual GCode. This is the only class that knows how GCode looks and feels.
//  Any customizations on GCodes flavors are done in this class.
class GCodeExport
{
private:
    FILE* f;
    double extrusionAmount;
    double extrusionPerMM;
    double retractionAmount;
    double retractionAmountPrime;
    int retractionZHop;
    double extruderSwitchRetraction;
    double minimalExtrusionBeforeRetraction;
    double extrusionAmountAtPreviousRetraction;
    Point3 currentPosition;
    Point extruderOffset[MAX_EXTRUDERS];
    char extruderCharacter[MAX_EXTRUDERS];
    double currentSpeed;
    int retractionSpeed;
    int zPos;
    bool isRetracted;
    int extruderNr;
    int currentFanSpeed;
    int flavor;
    
    double totalFilament[MAX_EXTRUDERS];
    double totalPrintTime;
    TimeEstimateCalculator estimateCalculator;

    double firstLineSection;
public:
    
    GCodeExport();
    
    ~GCodeExport();
    
    void replaceTagInStart(const char* tag, const char* replaceValue);
    
    void setExtruderOffset(int id, Point p);
    
    void setFlavor(int flavor);
    int getFlavor();
    
    void setFilename(const char* filename);
    
    bool isOpened();
    
    void setExtrusion(int layerThickness, int filamentDiameter, int flow);
    
    void setRetractionSettings(int retractionAmount, int retractionSpeed, int extruderSwitchRetraction, int minimalExtrusionBeforeRetraction, int zHop, int retractionAmountPrime);
    
    void setZ(int z);
    
    void setFirstLineSection(int initialLayerThickness, int filamentDiameter, int filamentFlow, int layer0extrusionWidth);

    Point getPositionXY();
    
    int getPositionZ();

    int getExtruderNr();
    
    double getTotalFilamentUsed(int e);

    double getTotalPrintTime();
    void updateTotalPrintTime();
    
    void writeComment(const char* comment, ...);

    void writeLine(const char* line, ...);
    
    void resetExtrusionValue();
    
    void writeDelay(double timeAmount);
    
    void writeMove(Point p, double speed, int lineWidth);
    
    void writeRetraction();
    
    void switchExtruder(int newExtruder);
    
    void writeCode(const char* str);
    
    void writeFanCommand(int speed);
    
    void finalize(int maxObjectHeight, int moveSpeed, const char* endCode);

    int getFileSize();
    void tellFileSize();
};

//The GCodePathConfig is the configuration for moves/extrusion actions. This defines at which width the line is printed and at which speed.
class GCodePathConfig
{
public:
    int speed;
    int lineWidth;
    const char* name;
    bool spiralize;
    int clip;
    
    GCodePathConfig() : speed(0), lineWidth(0), name(nullptr), spiralize(false), clip(0) {}
    GCodePathConfig(int speed, int lineWidth, const char* name) : speed(speed), lineWidth(lineWidth), name(name), spiralize(false), clip(0) {}
    
    void setData(int speed, int lineWidth, const char* name)
    {
        this->speed = speed;
        this->lineWidth = lineWidth;
        this->name = name;
    }

    /**
     * New method to support clip distance value.
     * - Clip value comes from SETTINGS
     * - This method is called by inset0Config and insetXConfig
     */
    void setData(int speed, int lineWidth, int clip, const char* name)
    {
        this->speed = speed;
        this->lineWidth = lineWidth;
        this->name = name;
        this->clip = clip;
    }
};

class GCodePathSegment
{
public:
    Point pos;
    double length;
    double speed;
    int lineWidth;

    GCodePathSegment(Point p0, Point p1, GCodePathConfig * config) : pos(p1), length(vSizeMM(p1 - p0)), speed(config->speed), lineWidth(config->lineWidth) {}
};

class GCodePath
{
public:
    GCodePathConfig* config;
    bool retract;
    int extruder;
    vector<GCodePathSegment> segments;
    bool done;//Path is finished, no more moves should be added, and a new path should be started instead of any appending done to this one.
};

//The GCodePlanner class stores multiple moves that are planned.
// It facilitates the combing to keep the head inside the print.
// It also keeps track of the print time estimate for this planning so speed adjustments can be made.
class GCodePlanner
{
private:
    Point startPosition;
    Point lastPosition;
    vector<GCodePath> paths;
    Comb* comb;
    
    GCodePathConfig travelConfig;
    int currentExtruder;
    int retractionMinimalDistance;
    bool forceRetraction;
    bool alwaysRetract;
    double extraTime;
    double totalPrintTime;
    bool layer0Retract;
private:
    GCodePath* getLatestPathWithConfig(GCodePathConfig* config);
    void forceNewPathStart();
    void simpleTimeEstimate(double &travelTime, double &extrudeTime);
    void addPointWithConfig(Point p, GCodePathConfig* config);
public:
    GCodePlanner(Point startPositionXY, int startExtruder, int travelSpeed, int retractionMinimalDistance);
    ~GCodePlanner();
    
    bool setExtruder(int extruder)
    {
        if (extruder == currentExtruder)
            return false;
        currentExtruder = extruder;
        return true;
    }
    
    int getExtruder()
    {
        return currentExtruder;
    }

    void setCombBoundary(Polygons* polygons)
    {
        if (comb)
            delete comb;
        if (polygons)
            comb = new Comb(*polygons);
        else
            comb = nullptr;
    }
    
    void setAlwaysRetract(bool alwaysRetract)
    {
        this->alwaysRetract = alwaysRetract;
    }
    
    void forceRetract()
    {
        forceRetraction = true;
    }
    
    int getCoolingSpeedFactor()
    {
        if (layerTime < layerTimeWithoutSpeedLimits) {
            return 100 * layerTime / layerTimeWithoutSpeedLimits;
        } else {
            return 100;
        }
    }

    Point getLastPosition()
    {
        return lastPosition;
    }

    int getLastExtruder()
    {
        return currentExtruder;
    }
    
    void addTravel(Point p);
    
    void addExtrusionMove(Point p, GCodePathConfig* config);
    
    void moveInsideCombBoundary(int distance);

    void addPolygon(PolygonRef polygon, int startIdx, GCodePathConfig* config);

    void addPolygonsByOptimizer(Polygons& polygons, GCodePathConfig* config);
    
    void enforceSpeedLimits(double minTime, int minSpeed, int maxSpeed);

    double limitFlowGrowthRate(double initialFlow2D, double maxFlowGrowthRate, bool forward);
    
    void writeGCode(bool liftHeadIfNeeded, int layerThickness);

    void writeGCode(bool liftHeadIfNeeded, int layerThickness, int layerNr);

    void setLayer0Retract(bool _layer0Retract);
};

}//namespace cura

#endif//GCODEEXPORT_H

