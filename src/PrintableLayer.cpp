#include "PrintableLayer.h"

namespace cura
{

PrintableLayer::PrintableLayer(SliceDataStorage& storage, unsigned int layer_nr)
{
    this->z = storage.meshes.back().layers.at(layer_nr).sliceZ;
    
    // TODO: think of an elegant way to create PrintableLayerParts in-place because emplace_back wont work because the layerparts are a virtual class
    // TODO: fix hardcoding 
    for (unsigned int mesh_idx = 0; mesh_idx < storage.meshes.size(); mesh_idx++)
    {
        // TODO: check that it is indeed the case that equal layer numbers guarantee equal z heights!!
        assert(storage.meshes.at(mesh_idx).layers.at(layer_nr).sliceZ == this->z);
        for (unsigned int part_idx = 0; part_idx < storage.meshes.at(mesh_idx).layers.at(layer_nr).parts.size(); part_idx++)
        {
            // TODO: fix memory leak:
            PrintableMeshLayerPart* newpart = new PrintableMeshLayerPart(storage, mesh_idx, layer_nr, part_idx);
            this->parts.push_back(newpart);
//            this->parts.push_back(part);
        }
    }
}

} // namespace cura