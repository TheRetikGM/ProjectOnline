#pragma once
#include <glm/glm.hpp>
#include <string>
#include <cstdint>
#include "Ren/KeyInterface.hpp"
#include "Scene.h"

namespace Ren
{
    class NativeScriptSystem;

    class NativeScript
    {
    public:
        virtual ~NativeScript() {}

        virtual void OnInit() {}
        virtual void OnDestroy() {}
        virtual void OnUpdate(float dt) {}
        // TODO:
        // void OnFixedUpdate(float dt) {}
        // void OnCollision(...) {}
    protected:
        KeyInterface* m_input;
        Entity& self = m_entity;
        
        template<typename T>
        inline T& GetComponent() { self.Get<T>(); }
        inline bool KeyPressed(const SDL_KeyCode& key) { return m_input->KeyPressed(key); }
        inline bool KeyHeld(const SDL_KeyCode& key) { return m_input->KeyHeld(key); }
    private:
        Entity m_entity;

        friend class Ren::NativeScriptSystem;
    };
}
