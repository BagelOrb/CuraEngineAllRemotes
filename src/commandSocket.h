#ifndef COMMAND_SOCKET_H
#define COMMAND_SOCKET_H

#include "utils/socket.h"
#include "utils/polygon.h"
#include "settings/settings.h"
#include "progress/Progress.h"
#include "PrintFeature.h"

namespace cura
{

class CommandSocket
{
private:
    static std::unique_ptr<CommandSocket> instance_; //!< always instantiated

public:
    static CommandSocket* getInstance();
    static void setInstance(std::unique_ptr<CommandSocket> instance);

protected:
    CommandSocket() = default;

#ifdef ARCUS
    /*! 
     * Handler for ObjectList message. 
     * Loads all objects from the message and starts the slicing process
     * 
     * Also handles meshgroup settings and extruder settings.
     * 
     * \param[in] list The list of objects to slice
     * \param[in] settings_per_extruder_train The extruder train settings to load into the meshgroup
     */
    void handleObjectList(cura::proto::ObjectList* list, const google::protobuf::RepeatedPtrField<cura::proto::Extruder> settings_per_extruder_train);
#endif

    /*!
     * Send info on an optimized layer to be displayed by the forntend: set the z and the thickness of the layer.
     */
    void sendOptimizedLayerInfo(int layer_nr, int32_t z, int32_t height);

    /*! 
     * Send a polygon to the front-end. This is used for the layerview in the GUI
     */
    static void sendPolygons(cura::PrintFeatureType type, const cura::Polygons& polygons, int line_width);

    /*! 
     * Send a polygon to the front-end. This is used for the layerview in the GUI
     */
    static void sendPolygon(cura::PrintFeatureType type, Polygon& polygon, int line_width);

    /*!
     * Send a line to the front-end. This is used for the layerview in the GUI
     */
    static void sendLineTo(cura::PrintFeatureType type, Point to, int line_width);

    /*!
     * Set the current position of the path compiler to \p position. This is used for the layerview in the GUI
     */
    static void setSendCurrentPosition(Point position);

    /*!
    * Set which layer is being used for the following calls to SendPolygons, SendPolygon and SendLineTo.
    */
    static void setLayerForSend(int layer_nr);

     /*!
     * Set which extruder is being used for the following calls to SendPolygons, SendPolygon and SendLineTo.
     */
    static void setExtruderForSend(int extruder);

    /*!
     * Send a polygon to the front-end if the command socket is instantiated. This is used for the layerview in the GUI
     */
    static void sendPolygonsToCommandSocket(cura::PrintFeatureType type, int layer_nr, const cura::Polygons& polygons, int line_width);

    /*! 
     * Send progress to GUI
     */
    virtual void sendProgress(float amount) {}
    
    /*!
     * Send the current stage of the process to the GUI (starting, slicing infill, etc) 
     */
    virtual void sendProgressStage(Progress::Stage stage) {}
    
    /*!
     * Send time estimate of how long print would take.
     */
    void sendPrintTimeMaterialEstimates();
    
    /*!
     * Does nothing at the moment
     */
    void sendPrintMaterialForObject(int index, int extruder_nr, float material_amount);
    
    /*!
     * Send the slices of the model as polygons to the GUI.
     *
     * The GUI may use this to visualize the early result of the slicing
     * process.
     */
    void sendLayerData();

    /*!
     * Send the sliced layer data to the GUI after the optimization is done and
     * the actual order in which to print has been set.
     *
     * The GUI may use this to visualize the g-code, so that the user can
     * inspect the result of slicing.
     */
    void sendOptimizedLayerData();

    /*!
     * \brief Sends a message to indicate that all the slicing is done.
     *
     * This should indicate that no more data (g-code, prefix/postfix, metadata
     * or otherwise) should be sent any more regarding the latest slice job.
     */
    virtual void sendFinishedSlicing() {}

    virtual void beginGCode() {}
    
    /*!
     * Flush the gcode in gcode_output_stream into a message queued in the socket.
     */
    void flushGcode();
    void sendGCodePrefix(std::string prefix);

#ifdef ARCUS
private:
    class Private;
    const std::unique_ptr<Private> private_data;
    class PathCompiler;
    const std::unique_ptr<PathCompiler> path_comp;
#endif
};

}//namespace cura

#endif//COMMAND_SOCKET_H
