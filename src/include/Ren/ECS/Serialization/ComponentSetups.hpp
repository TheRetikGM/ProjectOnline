#pragma once

#include "Ren/ECS/Components.h"
#include "Ren/Utils/Logger.hpp"

namespace Ren
{
    // Function to help user to customize, how a component should be initialized from already existing component data.
    // For ex. SpriteComponent needs to be constructed with a path to texture.
    // So, this function is used to say: how a component should be created in a scene
    // Use case: 
    //      template<>
    //      void setup_component<SpriteComponent>(Entity ent, const SpriteComponent& data) { ... }
    template<typename T>
    void setup_component(Entity ent, const T& data)
    {
        LOG_E("Component deserialization setup is not defined. Typename = " + Utils::type_name<T>());
    }

    template<>
    void setup_component<TransformComponent>(Entity e, const TransformComponent& t)
    {
        // All entities have transform component added by default when created.
        auto& trans = e.Get<TransformComponent>();
        trans.position = t.position;
        trans.scale = t.scale;
        trans.rotation = t.rotation;
        trans.layer = t.layer;
    }
    template<>
    void setup_component<SpriteComponent>(Entity e, const SpriteComponent& comp)
    {
        auto& s = e.Add<SpriteComponent>(comp.img_path.c_str());
        s.m_Color = comp.m_Color;
        s.m_PixelsPerUnit = comp.m_PixelsPerUnit;
    }
    template<>
    void setup_component<RigidBodyComponent>(Entity e, const RigidBodyComponent& comp)
    {
        auto& r = e.Add<RigidBodyComponent>();
        r.body_def = comp.body_def;
        r.fixture_def = comp.fixture_def;
        r.p_shape = comp.p_shape;
    }
};