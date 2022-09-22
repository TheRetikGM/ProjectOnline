#pragma once
#include <glm/glm.hpp>
#include <string>
#include <cstdint>
#include <filesystem>
#include <box2d/box2d.h>
#include <typeinfo>

#include "Loaders.hpp"

#define UNDEFINED_TAG "undefined"
#define UNDEFINED_PATH "undefined_path"

namespace Ren
{
    namespace Utils
    {
        template<typename T>
        static std::string type_name() { return typeid(T).name(); }
    }

    struct TransformComponent
    {
        glm::vec2 position{ .0f, .0f };
        glm::vec2 scale { .0f, .0f };
        // Rotation in degrees ccw from positive x.
        float rotation{ 0.0f };
        int32_t layer{ 0 };

        // Use this if entity has RigidBodyComponent and you want to enforce transform as position.
        bool dirty = false;
        
        TransformComponent(glm::vec2 pos = glm::vec2(0.0f), glm::vec2 scale = glm::vec2(10.0f), int32_t layer = 0)
            : position(pos), scale(scale), layer(layer) {}
    };

    // Base class for components, that has to load some texture.
    struct ImgComponent
    {
        std::filesystem::path img_path = UNDEFINED_PATH; 
        TextureHandle texture_handle;

        ImgComponent() = default;
        ImgComponent(std::filesystem::path path) : img_path(path), texture_handle() {}

        inline SDL_Texture* GetTexture() { return (texture_handle.second) ? texture_handle.second->texture : nullptr; };
        inline entt::resource<TextureResource> GetTextureResource() { return texture_handle.second; }
    };

    class SpriteComponent : public ImgComponent
    {
    public:
        glm::ivec3 m_Color = glm::ivec3(255);
        // Texture size is defined in pixels, so we have to scale it to match our desired size in unit-space.
        // However we don't use pixels-per-unit from Renderer::GetCamera(), because we don't want the texture
        // to have same size on different zoom levels of the camera (defined by the ppu ratio).
        glm::ivec2 m_PixelsPerUnit = glm::ivec2(200);

        SpriteComponent(std::filesystem::path img_path = UNDEFINED_PATH, glm::vec3 color = glm::vec3(255)) : m_Color(color), ImgComponent(img_path) {}

        // Get size in units (accounting for m_PixelsPerUnit)
        glm::vec2 GetSize();
    };

    // TODO support for multiple fixtures
    struct RigidBodyComponent
    {
        b2BodyDef body_def{};
        b2Body* p_body{ nullptr };
        Ref<b2Shape> p_shape;
        // Note: Use p_shape. Do NOT use b2FixtureDef::shape as it will be set when component is initialized.
        b2FixtureDef fixture_def{};
    };

    ///////////////////////////////////////
    ////////////// Scripting //////////////
    ///////////////////////////////////////

    class NativeScript;
    struct NativeScriptComponent
    {
        NativeScript* script_instance{ nullptr };

        template<typename T>
        void Bind()
        {
            REN_ASSERT(script_instance == nullptr, "There is already bound script to this component. Multiple bound scripts are not supported yet.");
            script_instance = dynamic_cast<NativeScript*>(new T());
        }

        void Unbind();
    };

} // namespace Ren