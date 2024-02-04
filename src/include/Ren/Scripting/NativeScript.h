/**
 * @file Ren/Scripting/NativeScript.hpp
 * @brief Declaration of NativeScript.
 */

#pragma once
#include <glm/glm.hpp>
#include <string>
#include <cstdint>
#include <box2d/box2d.h>

#include "Ren/Core/Input.h"
#include "Ren/ECS/Scene.h"

namespace Ren {
    class NativeScriptSystem;

    class NativeScript {
    public:
        virtual ~NativeScript() {}

        virtual void OnInit() {}
        virtual void OnDestroy() {}
        virtual void OnUpdate(float dt) {}

        // Box2D collision callbacks.
        virtual void OnContactBegin(Entity e, b2Contact* contact) {}
        virtual void OnContactEnd(Entity e, b2Contact* contact) {}
        virtual void OnContactPreSolve(Entity e, b2Contact* contact, const b2Manifold* old_manifold) {}
        virtual void OnContactPostSolve(Entity e, b2Contact* contact, const b2ContactImpulse* impulse) {}

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
