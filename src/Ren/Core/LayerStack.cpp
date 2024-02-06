/**
 * @file Ren/Core/LayerStack.cpp
 * @brief Implementation of layer architecture.
 */

#include <algorithm>

#include "Ren/Core/LayerStack.hpp"
#include "Ren/Core/Layer.hpp"

using namespace Ren;

LayerStack::LayerStack()
    : m_layersAndOverlays()
    , m_overlayBegin(m_layersAndOverlays.end()) { }

LayerStack::Stack::iterator LayerStack::findLayerWithName(const std::string& name) {
    return std::find_if(m_layersAndOverlays.begin(), m_layersAndOverlays.end(), [&name](const Ref<Layer>& l){
        return l->GetName() == name;
    });
}
void LayerStack::PushLayer(Ref<Layer> layer) {
    // Check if layer with given name already exists.
    auto it = findLayerWithName(layer->GetName());
    REN_ASSERT(it == m_layersAndOverlays.end(), "Layer with name '" + layer->GetName() + "' already exists.");

    if (it == m_layersAndOverlays.end())
        m_layersAndOverlays.insert(m_overlayBegin, layer);
}
void LayerStack::PushOverlay(Ref<Layer> layer) {
    auto it = m_layersAndOverlays.insert(m_layersAndOverlays.end(), layer);

    if (m_overlayBegin == m_layersAndOverlays.end())
        m_overlayBegin = it;
}
void LayerStack::DeleteLayer(const std::string& name) {
    auto it = findLayerWithName(name);
    REN_ASSERT(it != m_layersAndOverlays.end(), "Cannot find layer with name '" + name + "'.");

    if (it != m_layersAndOverlays.end())
        m_layersAndOverlays.erase(it);
}
void LayerStack::LayerEnabled(const std::string& name, bool b) {
    auto it = findLayerWithName(name);
    REN_ASSERT(it != m_layersAndOverlays.end(), "Cannot find layer with name '" + name + "'.");

    if (it != m_layersAndOverlays.end())
        (*it)->m_Enabled = b;
}

Ref<Layer> LayerStack::GetLayer(const std::string& name) {
    auto it = findLayerWithName(name);
    REN_ASSERT(it != m_layersAndOverlays.end(), "Cannot find layer with name '" + name + "'.");

    if (it != m_layersAndOverlays.end())
        return *it;

    return nullptr;
}
