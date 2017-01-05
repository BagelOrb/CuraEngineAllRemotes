/* 
 * File:   Towering.h
 * Author: sjoerd
 *
 * Created on March 3, 2016, 8:55 PM
 */

#ifndef TOWERING_H
#define TOWERING_H

#include "PrintableLayer.h"


namespace cura
{

/*! Find the best order to process layerpart. This includes finding towers.
 *  \param layers the list of layers of parts
 */
    
class Towering 
{
public:
    Towering(std::vector<PrintableLayer>& layers);
    bool hasNext();
   
    PrintableLayerPart* getNextPart(Point last_extruder_location);
   
    std::list<PrintableLayerPart *> getNextGroup(Point last_extruder_location);
    
    
private:
//    std::list<std::list<PrintableLayerPart *>> groups;
    std::vector<PrintableLayer>& layers;
    int64_t max_generated_z = 0;
    
    
    
    /*! Calculate the overlap area between two parts. This is no longer used. Was used to estimate the 'fitness' of the next part given the pevious part. Currently we simply check that the last_extruder_location is inside the part.
     * \param part1 first part
     * \param part2 second part
     * 
     * \return area of the intersection of the parts
     */
    static double layerPartOverlapArea(PrintableLayerPart& part1, PrintableLayerPart& part2);
    
    
    /*! Determines whether printing part1 now will cause problems with printing part2 later. I.e. whether part2 hangs over or touches part1.
     * \param part1 part to be printed now
     * \param part2 part to be printed later
     * \return whether this order of printing causes problems
     */
    static bool partBlocksOtherPart(PrintableLayerPart& part1, PrintableLayerPart& part2);
    
    
    /*! Determines whether a part can be printed without obstructing future parts.
     * \param part the part that is to be printed
     * \param layers the collection of layers to check for collisions
     * \return whether this part can be printed now.
     */
    static bool layerPartCanBePrintedNext(PrintableLayerPart& part,std::vector<PrintableLayer>& layers);
    
    
    /*! From the list of layers, find a layerpart that fits nicely on to of the last extruded location.
     * \param layers list of PrintableLayers too choose from.
     * \param max_z pick a layerpart that is not above this value. used for speeding up the process.
     * \param last_extruder_location location of the extruder.
     * 
     * \return boolean, whether a part was found
     */
    static bool processNextPart(std::vector< PrintableLayer >& layers, int64_t max_z, Point last_extruder_location);
};

}

#endif /* TOWERING_H */

