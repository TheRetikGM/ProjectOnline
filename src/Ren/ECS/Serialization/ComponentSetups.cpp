/**
 * @file Ren/ECS/Serialization/ComponentSetups.cpp
 * @brief Implementation of component setups.
 */

#include "Ren/ECS/Serialization/ComponentSetups.h"

using namespace Ren;

template<>
void Ren::setup_component<TransformComponent>(Entity e, const TransformComponent& t) {
    // All entities have transform component added by default when created.
    auto& trans = e.Get<TransformComponent>();
    trans.position = t.position;
    trans.scale = t.scale;
    trans.rotation = t.rotation;
    trans.layer = t.layer;
}

template<>
void Ren::setup_component<SpriteComponent>(Entity e, const SpriteComponent& comp) {
    auto& s = e.Add<SpriteComponent>(comp.img_path.c_str());
    s.m_Color = comp.m_Color;
    s.m_PixelsPerUnit = comp.m_PixelsPerUnit;
}

