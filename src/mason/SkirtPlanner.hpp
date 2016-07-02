/** Copyright (C) 2016 Scott Lenser - Released under terms of the AGPLv3 License */

#ifndef INCLUDED_SKIRT_PLANNER_HPP
#define INCLUDED_SKIRT_PLANNER_HPP

#include "BuildSubPlanner.hpp"

namespace mason {

class SkirtPlanner : public BuildSubPlanner {
public:
    SkirtPlanner();

    virtual ~SkirtPlanner();

    virtual const std::string &getName() const;
    
    virtual void process(BuildPlan *build_plan);

private:
    void writePolygonsToBuildPlan(
        const Polygons &polys,
        coord_t top_z, coord_t height, coord_t width,
        BuildPlan *build_plan);

    std::string m_name;
};

}

#endif
