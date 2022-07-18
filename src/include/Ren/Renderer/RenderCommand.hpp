#pragma once
#include <entt/entt.hpp>

#include "Ren/ECS/Scene.h"

namespace Ren
{
    struct RenderCommandPoly : entt::type_list<void(SDL_Renderer*), int() const> 
    {
        template<typename Base>
        struct type: Base {
            void Render(SDL_Renderer* renderer) { entt::poly_call<0>(*this, renderer); }
            int GetLayer() const { return entt::poly_call<1>(*this); }
        };

        template<typename Type>
        using impl = entt::value_list<&Type::Render, &Type::GetLayer>;
    };
    using RenderCommand = entt::poly<RenderCommandPoly>;

    struct SpriteRenderCommand
    {
        TransformComponent& transform;
        SpriteComponent& sprite;

        SpriteRenderCommand(Ren::Entity ent)
            : transform(ent.Get<Ren::TransformComponent>())
            , sprite(ent.Get<Ren::SpriteComponent>())
        {
        }

        void Render(SDL_Renderer* renderer)
        {
            SDL_Rect rect{ (int)transform.position.x, (int)transform.position.y, (int)transform.scale.x, (int)transform.scale.y };
            SDL_SetTextureColorMod(sprite.GetTexture(), sprite.color.r, sprite.color.g, sprite.color.b);
            SDL_RenderCopyEx(renderer, sprite.GetTexture(), nullptr, &rect, transform.rotation, nullptr, {});
        }
        int GetLayer() const
        {
            return transform.layer;
        }
    };
}