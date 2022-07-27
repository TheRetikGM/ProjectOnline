#pragma once
#include <entt/entt.hpp>

#include "Ren/ECS/Scene.h"
#include "Ren/Renderer/Camera.h"

namespace Ren
{
    // Define base class for render commands using static polymorphism implemented using EnTT::poly
    // This class defines that when some structure has Render() and GetLayer() member functions, they can be used as render commands.
    // --> For details check docs: https://github.com/skypjack/entt/wiki/Crash-Course:-poly
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

    // Basic Commands //

    // // Command used to render a sprite.
    // struct SpriteRenderCommand
    // {
    //     TransformComponent& transform;
    //     SpriteComponent& sprite;

    //     SpriteRenderCommand(Ren::Entity ent)
    //         : transform(ent.Get<Ren::TransformComponent>())
    //         , sprite(ent.Get<Ren::SpriteComponent>())
    //     {
    //     }

    //     void Render(SDL_Renderer* renderer)
    //     {
    //         Rect rrect{ transform.position.x, transform.position.y, transform.scale.x, transform.scale.y };
    //         SDL_Rect rect = camera->ConvertRect(rrect);
    //         SDL_SetTextureColorMod(sprite.GetTexture(), sprite.color.r, sprite.color.g, sprite.color.b);
    //         SDL_RenderCopyEx(renderer, sprite.GetTexture(), nullptr, &rect, transform.rotation, nullptr, {});
    //     }
    //     int GetLayer() const
    //     {
    //         return transform.layer;
    //     }
    // };
}