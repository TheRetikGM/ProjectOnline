#pragma once
#include <glm/glm.hpp>
#include <string>
#include <cstdint>

#include "Ren/Core/KeyInterface.hpp"
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
        inline T& GetComponent() { return self.Get<T>(); }
        inline bool HasTag(std::string tag) { return m_entity.p_scene->HasTag(m_entity, tag); }
        inline void AddTag(std::string tag) { m_entity.p_scene->AddTag(m_entity, tag); }
        inline void RemTag(std::string tag) { m_entity.p_scene->RemTag(m_entity, tag); }
        inline bool KeyPressed(Key key) { return m_input->KeyPressed(key); }
        inline bool KeyHeld(Key key) { return m_input->KeyHeld(key); }
    private:
        Entity m_entity;

        friend class Ren::NativeScriptSystem;
    };
}
