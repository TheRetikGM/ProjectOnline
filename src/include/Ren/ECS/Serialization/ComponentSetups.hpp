/**
 * @file Ren/ECS/Serialization/ComponentSetups.hpp
 * @brief Component setup declarations.
 *
 * Some components need a special setup, when they are created from an existing data. This happens during serialization.
 */

#pragma once

#include "Ren/ECS/Scene.hpp"
#include "Ren/ECS/Components.hpp"

namespace Ren {
    // Function to help user to customize, how a component should be initialized from already existing component data.
    // For ex. SpriteComponent needs to be constructed with a path to texture.
    // So, this function is used to say: how a component should be created in a scene
    // Use case:
    //      template<>
    //      void setup_component<SpriteComponent>(Entity ent, const SpriteComponent& data) { ... }
    template<typename T>
    void setup_component(Entity ent, const T& data) {
        // Try copy construction
        auto& r = ent.Add<T>();
        r = data;
    }
    template<> void setup_component<TransformComponent>(Entity e, const TransformComponent& t);
    template<> void setup_component<SpriteComponent>(Entity e, const SpriteComponent& comp);
};
