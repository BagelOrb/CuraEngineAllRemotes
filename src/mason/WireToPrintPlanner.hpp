/** Copyright (C) 2016 Scott Lenser - Released under terms of the AGPLv3 License */

#ifndef INCLUDED_WIRE_TO_PRINT_PLANNER_HPP
#define INCLUDED_WIRE_TO_PRINT_PLANNER_HPP

#include "BuildSubPlanner.hpp"

namespace mason {

class WireToPrintPlanner : public BuildSubPlanner {
public:
    virtual ~WireToPrintPlanner();

    virtual void process(BuildPlan *build_plan);
};

}

#endif
