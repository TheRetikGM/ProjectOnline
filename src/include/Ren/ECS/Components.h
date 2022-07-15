#pragma once
#include <glm/glm.hpp>
#include <string>
#include <cstdint>
#include <filesystem>
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

    struct TagComponent
    {
        std::string tag = UNDEFINED_TAG;

        TagComponent() = default;
        TagComponent(const std::string& tag_name) : tag(tag_name) {}
    };

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
        glm::ivec4 color = glm::ivec4(255);

        SpriteComponent(std::filesystem::path img_path = UNDEFINED_PATH, glm::vec4 color = glm::vec4(255)) : color(color), ImgComponent(img_path) {}
    };
}