#pragma once
#include <vector>
#include <list>
#include <string>

#include "Core.h"
#include "GameCore.h"
#include "RenSDL/Event.hpp"
#include "RenSDL/Context.hpp"
#include "KeyInterface.hpp"

namespace Ren
{
    class Layer
    {
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
        inline bool KeyPressed(const SDL_KeyCode& key) { return m_GameCore->m_Input.KeyPressed(key); }
        inline bool KeyHeld(const SDL_KeyCode& key) { return m_GameCore->m_Input.KeyHeld(key); }
        inline KeyInterface* GetInput() { return &m_GameCore->m_Input; }
        inline SDL_Renderer* GetRenderer() { return m_GameCore->GetRenderer(); }
    private:
        // Name of the layer.
        std::string m_name{ "Layer" };
    
        friend class Ren::GameCore;
    };
} // namespace Ren
