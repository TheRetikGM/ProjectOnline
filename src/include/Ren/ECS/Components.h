#pragma once
#include <glm/glm.hpp>
#include <string>
#include <cstdint>
#include <filesystem>
#include <box2d/box2d.h>

#include "Loaders.hpp"

#define UNDEFINED_TAG "undefined"
#define UNDEFINED_PATH "undefined_path"

namespace Ren
{
    struct TransformComponent
    {
        glm::vec2 position{ .0f, .0f };
        glm::vec2 scale { .0f, .0f };
        float rotation{ 0.0f };
        int32_t layer{ 0 };
        
        TransformComponent(glm::vec2 pos = glm::vec2(0.0f), glm::vec2 scale = glm::vec2(10.0f), int32_t layer = 0)
            : position(pos), scale(scale), layer(layer) {}
    };

    // TODO: Support multiple tags.
    struct TagComponent
    {
        std::string tag = UNDEFINED_TAG;

        TagComponent() = default;
        TagComponent(const std::string& tag_name) : tag(tag_name) {}
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

    struct SpriteComponent : public ImgComponent
    {
        glm::ivec3 color = glm::ivec3(255);

        SpriteComponent(std::filesystem::path img_path = UNDEFINED_PATH, glm::vec3 color = glm::vec3(255)) : color(color), ImgComponent(img_path) {}
    };

    struct RigidBodyComponent
    {
        b2BodyDef body_def{};
        b2Body* p_body{ nullptr };
        b2Shape* p_shape;
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