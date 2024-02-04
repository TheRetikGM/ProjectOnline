/**
 * @file Ren/Core/Layer.hpp
 * @brief Declaration of a single layer.
 *
 * Single layer in Ren engine means an object that can be updated in given layer each frame. All functionality should be implemented inside layers.
 */

#pragma once
#include <vector>
#include <list>
#include <string>

#include "Ren/Core/Core.h"
#include "Ren/Core/GameCore.h"
#include "Ren/RenSDL/Event.hpp"
#include "Ren/RenSDL/Context.hpp"
#include "Ren/Core/Input.h"

namespace Ren {
    class Layer {
    public:
        GameCore* m_GameCore{ nullptr };
        bool m_Enabled{ true };

        Layer(const std::string& name) : m_name(name) {}
        virtual ~Layer() {}

        virtual void OnInit() {}
        virtual void OnDestroy() {}
        virtual void OnEvent(Ren::Event& e) {}
        virtual void OnUpdate(float dt) {}
        virtual void OnRender(SDL_Renderer* renderer) {}
        virtual void OnImGui(Ren::ImGuiContext& context) {}

        inline const std::string& GetName() const { return m_name; }
        inline bool KeyPressed(Key key) { return m_GameCore->m_Input.KeyPressed(key); }
        inline bool KeyHeld(Key key) { return m_GameCore->m_Input.KeyHeld(key); }
        inline KeyInterface* GetInput() { return &m_GameCore->m_Input; }
        inline SDL_Renderer* GetRenderer() { return m_GameCore->GetRenderer(); }
    private:
        // Name of the layer.
        std::string m_name{ "Layer" };

        friend class Ren::GameCore;
    };
} // namespace Ren
