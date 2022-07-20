#pragma once
#include <list>
#include <string>

#include "Ren/Core/Core.h"

namespace Ren
{
    class Layer;

    class LayerStack
    {
        using Stack = std::list<Ref<Layer>>;
    public:
        LayerStack();

        void PushLayer(Layer* layer);
        void PushOverlay(Layer* overlay);

        // Delete layer from layer stack.
        void DeleteLayer(const std::string& name);
        // Set enable state of layer.
        void LayerEnabled(const std::string& name, bool b);
        // Return shared pointer to the layer, so that user can have the ability to remove layer without it beiing deleted.
        Ref<Layer> GetLayer(const std::string& name);

        inline Stack::iterator begin() { return m_layersAndOverlays.begin(); }
        inline Stack::iterator end() { return m_layersAndOverlays.end(); }
        inline Stack::reverse_iterator rbegin() { return m_layersAndOverlays.rbegin(); }
        inline Stack::reverse_iterator rend() { return m_layersAndOverlays.rend(); }
        inline void clear() { m_layersAndOverlays.clear(); m_overlayBegin = m_layersAndOverlays.end(); }

    private:
        // List that has structure: <all layers><all overlays>
        // -> so we store iterator to the first overlay element,
        // and push all normal layers before it and all overlays
        // after it.
        Stack m_layersAndOverlays;
        Stack::iterator m_overlayBegin;

        LayerStack::Stack::iterator findLayerWithName(const std::string& name);
    };
} // namespace Ren
